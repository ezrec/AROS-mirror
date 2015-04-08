/*************************************************************************\
 *                                                                       *
 * document.c                                                            *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <dos/dostags.h>
#include <dos/dosextens.h>
#include <dos/datetime.h>
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
#include "pageset.h"
#include "fontmap.h"
#include "docwindow.h"
#include "docgroup.h"
#include "pageslider.h"
#include "prefs.h"
#include "msgs.h"

#define DB(x)   //x
#define Static  static

#ifndef FORMAT_DEF
#   define FORMAT_DEF 4
#endif

#ifndef MAKE_ID
#   define MAKE_ID(a,b,c,d) ((ULONG)((a)<<24) | (ULONG)((b)<<16) | \
			     (ULONG)((c)<<8) | (ULONG)(d))
#endif

#if HAS_AA
#   define AA(x,y,z) x,y,z
#else
#   define AA(x,y,z)
#endif

const char *get_prt_dev(void);
const char *get_def_icon(void);

extern APTR oldwindowptr;
extern struct Process *proc;
extern Object *config;
extern const char *dispEntries[];

struct DocumentData {
    struct DocumentData *next;
    struct comm_info *comm_info;
    Object *obj;
    struct PDFDoc *pdfdoc;
    Object* fontmap;
    int win_count;
    int num_outlines;
    size_t outlines_bufsize;
    struct OutlinesInfo *outlines;
    Object *infowin;
    Object *prefswin;
    Object *pageprf;
    Object *zoomprf;
    Object *rotateprf;
    Object *textaaprf;
    Object *fillaaprf;
    Object *strokeaaprf;
    Object *leftprf;
    Object *topprf;
    Object *rightprf;
    Object *bottomprf;
    Object *columnsprf;
    Object *rowsprf;
    Object *continuousprf;
    Object *outlinesprf;
    Object *dispmodeprf;
    Object *toolbarprf;
    Object *navbarprf;
    Object *scanwin;
    Object *scanstartobj;
    Object *scanstopobj;
    Object *scanlv;
    Object *app;
    int open_textaa;
    int open_fillaa;
    int open_strokeaa;
    int open_columns;
    int open_rows;
    int open_continuous;
    int open_outlines;
    int open_dispmode;
    int open_toolbar;
    int open_navbar;
};
typedef struct DocumentData DocumentData;

static DocumentData *documents;

struct Screen *get_screen(Object* app);

Object *getDocument(struct comm_info *ci,Object* app,Object* win,BPTR lock,const char* name) {
    Object* doc=NULL;
    char *path;
    const char *endpath;
    BPTR dir=0;
    DocumentData *dat;
    char *oldtitle=NULL;
    LONG need_password=FALSE;
    BOOL retry=TRUE;
    char ownerpw[33];
    char userpw[33];
    struct Window *refwin;

    ownerpw[0]='\0';
    userpw[0]='\0';

    if(app) {
	struct List *list;
	Object *w;
	APTR state;
	set(app,MUIA_Application_Sleep,TRUE);
	get(app,MUIA_Application_WindowList,&list);
	state=list->lh_Head;
	if(w=NextObject(&state)) {
	    get(w,MUIA_Window_Window,&refwin);
	    if(refwin)
		proc->pr_WindowPtr=refwin;
	}
    }
    if(win) {
	APTR w;
	const char *t;
	get(win,MUIA_Window_Title,&oldtitle);
	set(win,MUIA_Window_Title,STR(MSG_LOADING));
    }
    endpath=PathPart((char*)name);
    if(endpath!=name) {
	path=MyAllocMem(endpath-name+1);
	if(path) {
	    BPTR olddir=CurrentDir(lock);
	    memcpy(path,name,endpath-name);
	    path[endpath-name]='\0';
	    dir=lock=Lock(path,ACCESS_READ);
	    CurrentDir(olddir);
	    MyFreeMem(path);
	    name=FilePart((char*)name);
	}
    }
    dat=documents;
    while(dat) {
	BPTR dir1;
	const char *name1;
	get(dat->obj,MYATTR_Document_Dir,&dir1);
	get(dat->obj,MYATTR_Document_Name,&name1);
	if(!Stricmp((char*)name1,(char*)name) &&
	   SameLock(dir1,lock)==LOCK_SAME) {
	    doc=dat->obj;
	    break;
	}
	dat=dat->next;
    }
    if(!doc) {
	do {
	    doc=DocumentObject,
		MYATTR_Document_CommInfo,ci,
		MYATTR_Document_Dir,lock,
		MYATTR_Document_Name,name,
		MYATTR_Document_NeedPassword,&need_password,
		MYATTR_Document_OwnerPW,ownerpw,
		MYATTR_Document_UserPW,userpw,
		MYATTR_Document_Application,app,
		End;
	    if(!doc && app && need_password) {
		Object *opwobj,*upwobj,*okobj,*cancelobj;
		Object *pwwin=WindowObject,
		    MUIA_Window_Title,STR(MSG_ENTER_PASSWORD_WIN),
		    MUIA_Window_ID,MAKE_ID('P','A','S','S'),
		    MUIA_Window_Screen,refwin?refwin->WScreen:NULL,
		    MUIA_Window_RootObject,VGroup,
			Child,ColGroup(2),
			    Child,Label2(STR(MSG_USERPW_GAD)),
			    Child,upwobj=StringObject,
				StringFrame,
				MUIA_String_MaxLen,33,
				MUIA_String_Secret,TRUE,
				MUIA_CycleChain,TRUE,
				End,
			    Child,Label2(STR(MSG_OWNERPW_GAD)),
			    Child,opwobj=StringObject,
				StringFrame,
				MUIA_String_MaxLen,33,
				MUIA_String_Secret,TRUE,
				MUIA_CycleChain,TRUE,
				End,
			    End,
			Child,HGroup,
			    Child,okobj=SimpleButton(STR(MSG_PASSWORD_OK)),
			    Child,cancelobj=SimpleButton(STR(MSG_PASSWORD_CANCEL)),
			    End,
			End,
		    End;

		if(pwwin) {
		    LONG opened;

		    //set(app,MUIA_Application_Sleep,TRUE);
		    DoMethod3(app,OM_ADDMEMBER,pwwin);

		    DoMethod8(pwwin,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			      app,2,MUIM_Application_ReturnID,1);
		    DoMethod8(okobj,MUIM_Notify,MUIA_Selected,FALSE,
			      app,2,MUIM_Application_ReturnID,2);
		    DoMethod8(cancelobj,MUIM_Notify,MUIA_Selected,FALSE,
			      app,2,MUIM_Application_ReturnID,1);
		    DoMethod8(opwobj,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
			      app,2,MUIM_Application_ReturnID,2);
		    DoMethod8(upwobj,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
			      app,2,MUIM_Application_ReturnID,2);
		    set(pwwin,MUIA_Window_Open,TRUE);
		    get(pwwin,MUIA_Window_Open,&opened);

		    if(opened) {
			LONG sigs=0;
			BOOL stop=FALSE;
			const char *buf;

			set(pwwin,MUIA_Window_ActiveObject,upwobj);

			do {
			    LONG id=DoMethod3(app,MUIM_Application_NewInput,&sigs);
			    switch(id) {
			      case MUIV_Application_ReturnID_Quit:
			      case 1:
				retry=FALSE;
				stop=TRUE;
				break;
			      case 2:
				stop=TRUE;
				get(opwobj,MUIA_String_Contents,&buf);
				strncpy(ownerpw,buf,sizeof(ownerpw));
				get(upwobj,MUIA_String_Contents,&buf);
				strncpy(userpw,buf,sizeof(userpw));
				break;
			    }
			    if(!stop && sigs)
				Wait(sigs);
			} while(!stop);

			set(pwwin,MUIA_Window_Open,FALSE);
		    }

		    DoMethod3(app,OM_REMMEMBER,pwwin);
		    MUI_DisposeObject(pwwin);

		    //set(app,MUIA_Application_Sleep,FALSE);
	       }
	    }
	} while(!doc && retry && need_password);
    }
    UnLock(dir);
    proc->pr_WindowPtr=oldwindowptr;
	if(win && !doc && oldtitle != NULL)
	set(win,MUIA_Window_Title,oldtitle);
    if(app)
	set(app,MUIA_Application_Sleep,FALSE);
    return doc;
}

void refresh_doc_fontmaps(void) {
    DocumentData *p=documents;
    while(p) {
	DoMethod2(p->obj,MYM_Document_UsePrefs);
	p=p->next;
    }
}


/*
 *  Server interface functions
 */

Static char* get_doc_info(struct comm_info * ci,struct PDFDoc *doc,int *linearized,int *encrypted,int *oktoprint,int *oktocopy) {
    char* r=NULL;
    struct msg_getdocinfo* p=ci->cmddata;
    sync();
    p->doc=doc;
    LOCK;/*init_exchange(ci);*/
    if((*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_GETDOCINFO)) {
	char* buf=NULL;
	size_t sz=p->size+1;
	char* q;
	*linearized=p->linearized;
	*encrypted=p->encrypted;
	*oktoprint=p->oktoprint;
	*oktocopy=p->oktocopy;
	if(sz>sizeof(union msg_max)) {
	    buf=(*ci->sv->MyAllocSharedMem)(ci->sv,sz);
	    if(buf)
		q=buf;
	    else {
		q=ci->cmddata;
		sz=sizeof(union msg_max);
	    }
	} else
	    q=ci->cmddata;
	(*ci->sv->exchange_raw_msg)(ci,q,sz,MSGID_GETDOCINFO);
	if(r=MyAllocMem(sz))
	    memcpy(r,q,sz);
	(*ci->sv->MyFreeSharedMem)(ci->sv,buf);
    }
    UNLOCK;/*end_exchange(ci);*/
    return r;
}

int has_outlines(struct comm_info *ci,struct PDFDoc *doc) {
    struct msg_get_outlines_info* p=ci->cmddata;
    int r;
    sync();
    p->doc=doc;
    r=(*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_HASOUTLINES);
    return r;
}

int get_outlines_info(struct comm_info *ci,struct PDFDoc *doc,int *num,size_t *bufsize) {
    struct msg_get_outlines_info* p=ci->cmddata;
    int r;
    sync();
    p->doc=doc;
    r=(*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_GETOUTLINESINFO);
    *num=p->num;
    *bufsize=p->bufsize;
    return r;
}

void get_outlines(struct comm_info *ci,struct PDFDoc *doc,struct OutlinesInfo *outlines,size_t bufsize) {
    struct msg_get_outlines* p=ci->cmddata;
    sync();
    p->doc=doc;
    LOCK;/*init_exchange(ci);*/
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_GETOUTLINES);
    (*ci->sv->exchange_raw_msg)(ci,outlines,bufsize,MSGID_GETOUTLINES);
    UNLOCK;/*end_exchange(ci);*/
}

Static struct FontOutputDev *init_scan(struct comm_info *ci) {
    struct msg_scan_fonts* p=ci->cmddata;
    sync();
    if((*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_INITSCAN))
	return p->out;
    return NULL;
}

Static int scan_fonts(struct comm_info *ci,struct PDFDoc *doc,
		      struct FontOutputDev *out,
		      int first_page,int last_page) {
    struct msg_scan_fonts* p=ci->cmddata;
    sync();
    p->doc=doc;
    p->out=out;
    p->first_page=first_page;
    p->last_page=last_page;
    return (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_SCANFONTS);
}

Static void end_scan_fonts(struct comm_info *ci,
			   struct FontOutputDev *out) {
    struct msg_scan_fonts* p=ci->cmddata;
    sync();
    p->out=out;
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_ENDSCANFONTS);
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

/*
 *  Box grabber class.
 */

static struct MUI_CustomClass *boxgrabber_mcc;

struct BoxGrabberData {
    DocumentData *docdat;
};
typedef struct BoxGrabberData BoxGrabberData;

#define BoxGrabberObject NewObject(boxgrabber_mcc->mcc_Class,NULL

Static ULONG bgNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    BoxGrabberData *dat;
    DocumentData *docdat=(APTR)GetTagData(MYATTR_PageSet_Document,0,msg->ops_AttrList);
    if(!docdat)
	return 0;
    if(obj=(Object *)DoSuperMethodA(cl,obj,(Msg)msg)) {
	ULONG d;
	dat=INST_DATA(cl,obj);
	dat->docdat=docdat;
    }
    return (ULONG)obj;
}

Static ULONG bgSet(struct IClass *cl,Object *obj,struct opSet *msg) {
    BoxGrabberData *dat=INST_DATA(cl,obj);
    struct TagItem *tags=msg->ops_AttrList;
    struct TagItem *tag;
    while(tag=NextTagItem(&tags)) {
	switch(tag->ti_Tag) {
	  case MYATTR_DocBitmap_CropBox: {
	    struct MYS_DocBitmap_CropBox *box=(APTR)tag->ti_Data;
	    DocumentData *d=dat->docdat;
	    set(d->leftprf,MUIA_String_Integer,box->left);
	    set(d->topprf,MUIA_String_Integer,box->top);
	    set(d->rightprf,MUIA_String_Integer,box->right);
	    set(d->bottomprf,MUIA_String_Integer,box->bottom);
	    break;
	  }
	}
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

Static ULONG bgGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    BoxGrabberData *dat=INST_DATA(cl,obj);
    switch(msg->opg_AttrID) {
      case MYATTR_DocBitmap_BoxGrabber:
	*msg->opg_Storage=(ULONG)dat->docdat->obj;
	return TRUE;
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

BEGIN_DISPATCHER(bgDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:  return bgNew(cl,obj,(APTR)msg);
      case OM_GET:  return bgGet(cl,obj,(APTR)msg);
      case OM_SET:  return bgSet(cl,obj,(APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(bgDispatcher)

/*
 *  Document Methods.
 */

Static ULONG docNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    struct comm_info *ci=(struct comm_info *)GetTagData(MYATTR_Document_CommInfo,0,msg->ops_AttrList);
    if(!ci)
	return 0;
    if(obj=(Object *)DoSuperMethodA(cl,obj,(Msg)msg)) {
	DocumentData *dat=INST_DATA(cl,obj);
	int n;
	dat->next=documents;
	documents=dat;
	dat->comm_info=ci;
	dat->obj=obj;
	dat->win_count=0;
	dat->fontmap=NULL;
	dat->infowin=NULL;
	dat->prefswin=NULL;
	dat->app=(Object*)GetTagData(MYATTR_Document_Application,0,msg->ops_AttrList);
	get(obj,MYATTR_Document_PDFDoc,&dat->pdfdoc);
	dat->outlines=NULL;
	dat->fontmap=FontMapObject,
	    End;
#if HAS_AA
	n=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_AntiAliasing);
	if(n)
	    n=*(int*)n;
	else
	    n=1;
	dat->open_textaa=(n&1)!=0;
	dat->open_fillaa=(n&256)!=0;
	dat->open_strokeaa=(n&65536)!=0;
#endif
	dat->num_outlines=-1;
	if(!has_outlines(dat->comm_info,dat->pdfdoc))
	    dat->num_outlines=0;
	n=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_Columns);
	dat->open_columns=n?*(int*)n:1;
	n=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_Rows);
	dat->open_rows=n?*(int*)n:1;
	n=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_Continuous);
	dat->open_continuous=n?*(int*)n:FALSE;
	n=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_DisplayMode);
	dat->open_dispmode=n?*(int*)n:DrPartialUpdates;
	get(obj,MYATTR_Document_NumPages,&n);
	dat->open_toolbar=TRUE;
	dat->open_navbar=n>1;
	n=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_OpenOutlines);
	dat->open_outlines=n?*(int*)n:FALSE;
	DoMethod2(obj,MYM_Document_LoadPrefs);
	DoMethod2(obj,MYM_Document_UsePrefs);
    }
    return (ULONG)obj;
}

Static ULONG docDispose(struct IClass *cl,Object *obj,Msg msg) {
    DocumentData *dat=INST_DATA(cl,obj);
    DocumentData **p=&documents;
    while(*p!=dat)
	p=&(*p)->next;
    *p=dat->next;
    if(dat->prefswin) {
	set(dat->prefswin,MUIA_Window_Open,FALSE);
	DoMethod3(dat->app,OM_REMMEMBER,dat->prefswin);
	MUI_DisposeObject(dat->prefswin);
    } else if(dat->fontmap)
	MUI_DisposeObject(dat->fontmap);
    if(dat->scanwin) {
	set(dat->scanwin,MUIA_Window_Open,FALSE);
	DoMethod3(dat->app,OM_REMMEMBER,dat->scanwin);
	MUI_DisposeObject(dat->scanwin);
    }
    if(dat->infowin) {
	set(dat->infowin,MUIA_Window_Open,FALSE);
	DoMethod3(dat->app,OM_REMMEMBER,dat->infowin);
	MUI_DisposeObject(dat->infowin);
    }
    if(dat->outlines)
	(*dat->comm_info->sv->MyFreeSharedMem)(dat->comm_info->sv,dat->outlines);
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG docGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    DocumentData *dat=INST_DATA(cl,obj);
    switch(msg->opg_AttrID) {
      case MYATTR_Document_HasOutlines:
	*msg->opg_Storage=dat->num_outlines;
	return TRUE;
      case MYATTR_Document_Outlines: {
	if(!dat->outlines && dat->num_outlines) {
	    size_t sz;
	    get_outlines_info(dat->comm_info,dat->pdfdoc,&dat->num_outlines,&dat->outlines_bufsize);
	    if(dat->num_outlines) {
		sz=sizeof(struct OutlinesInfo)+dat->outlines_bufsize+
(dat->num_outlines+1)*(sizeof(const char*)+sizeof(struct Action*));
		dat->outlines=(*dat->comm_info->sv->MyAllocSharedMem)(dat->comm_info->sv,sz);
		if(dat->outlines) {
		    dat->outlines->titles=(const char**)(dat->outlines+1);
		    dat->outlines->actions=(struct Action**)(dat->outlines->titles+dat->num_outlines+1);
		    dat->outlines->buf=(char*)(dat->outlines->actions+dat->num_outlines+1);
		    get_outlines(dat->comm_info,dat->pdfdoc,dat->outlines,sz);
		}
	    }
	}
	*msg->opg_Storage=(ULONG)dat->outlines;
	return TRUE;
      }
      case MYATTR_Document_OpenTextAA:
	*msg->opg_Storage=dat->open_textaa;
	break;
      case MYATTR_Document_OpenFillAA:
	*msg->opg_Storage=dat->open_fillaa;
	break;
      case MYATTR_Document_OpenStrokeAA:
	*msg->opg_Storage=dat->open_strokeaa;
	break;
      case MYATTR_Document_OpenColumns:
	*msg->opg_Storage=dat->open_columns;
	break;
      case MYATTR_Document_OpenRows:
	*msg->opg_Storage=dat->open_rows;
	break;
      case MYATTR_Document_OpenContinuous:
	*msg->opg_Storage=dat->open_continuous;
	break;
      case MYATTR_Document_OpenDispMode:
	*msg->opg_Storage=dat->open_dispmode;
	break;
      case MYATTR_Document_OpenToolBar:
	*msg->opg_Storage=dat->open_toolbar;
	break;
      case MYATTR_Document_OpenNavBar:
	*msg->opg_Storage=dat->open_navbar;
	break;
      case MYATTR_Document_OpenOutlines:
	*msg->opg_Storage=dat->open_outlines;
	break;
      case MYATTR_Document_Application:
	*msg->opg_Storage=(ULONG)dat->app;
	break;
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

Static ULONG docIncRef(struct IClass *cl,Object *obj) {
    DocumentData *dat=INST_DATA(cl,obj);
    return ++dat->win_count;
}

Static ULONG docDecRef(struct IClass *cl,Object *obj) {
    DocumentData *dat=INST_DATA(cl,obj);
    return --dat->win_count;
}

Static ULONG docOpenInfoWindow(struct IClass *cl,Object *obj) {
    DocumentData *dat=INST_DATA(cl,obj);
    if(!dat->infowin && dat->app) {
	int linearized,encrypted,oktoprint,oktocopy;
	char* str=get_doc_info(dat->comm_info,dat->pdfdoc,&linearized,&encrypted,&oktoprint,&oktocopy);
	if(str) {
	    char *p=str;
	    const char* name;
	    Object* grp;
	    get(obj,MYATTR_Document_Name,&name);
	    grp=ColGroup(2),End;
	    if(grp) {
		int k;
		int ok=TRUE;
		static const int labels[]={
		    MSG_INFO_TITLE,MSG_INFO_AUTHOR,MSG_INFO_SUBJECT,
		    MSG_INFO_CREATOR,MSG_INFO_PRODUCER,MSG_INFO_KEYWORDS,
		    MSG_INFO_CREATION_DATE,MSG_INFO_MODDATE,MSG_INFO_NUMPAGES,
		    MSG_INFO_LINEARIZED,MSG_INFO_ENCRYPTED,MSG_INFO_PRINTING,
		    MSG_INFO_COPY
		};

		for(k=0;k<13;++k) {
		    const char* q=p;
		    char buf[LEN_DATSTRING*2];
		    switch(k) {
		      case 6:
		      case 7:
			if(p[0]=='D' && p[1]==':') {
			    struct DateTime dt;
			    char* t;
			    BOOL ok=TRUE;
			    p+=2;
			    dt.dat_Format=FORMAT_CDN;
			    dt.dat_Flags=0;
			    dt.dat_StrDate=p;
			    if(!*p)
				ok=FALSE;
			    p+=strlen(p)+1;
			    dt.dat_StrTime=p;
			    if(!*p)
				ok=FALSE;
			    if(!StrToDate(&dt))
				ok=FALSE;
			    dt.dat_Format=FORMAT_DEF;
			    dt.dat_StrDay=NULL;
			    dt.dat_StrDate=buf;
			    dt.dat_StrTime=buf+LEN_DATSTRING;
			    if(ok) {
				DateToStr(&dt);
				t=buf+strlen(buf);
				*t++=' ';
				memmove(t,buf+LEN_DATSTRING,strlen(buf+LEN_DATSTRING)+1);
			    } else
				buf[0]='\0';
			    q=buf;
			} else
			    q=p;
			break;

		      case 8: {
			int num;
			get(obj,MYATTR_Document_NumPages,&num);
			sprintf(buf,"%d",num);
			q=buf;
			--p;
			break;
		      }
		      case 9:
			q=STR(linearized?MSG_INFO_YES:MSG_INFO_NO);
			--p;
			break;
		      case 10:
			q=STR(encrypted?MSG_INFO_YES:MSG_INFO_NO);
			--p;
			break;
		      case 11:
			q=STR(oktoprint?MSG_INFO_YES:MSG_INFO_NO);
			--p;
			break;
		      case 12:
			q=STR(oktocopy?MSG_INFO_YES:MSG_INFO_NO);
			--p;
			break;
		    }
		    if(*q) {
			Object *o=Label1(STR(labels[k]));
			if(!o) {
			    ok=FALSE;
			    break;
			}
			DoMethod3(grp,OM_ADDMEMBER,o);

			o=TextObject,
			    TextFrame,
			    MUIA_Background,MUII_TextBack,
			    MUIA_Text_Contents,q,
			    End;
			if(!o) {
			    ok=FALSE;
			    break;
			}
			DoMethod3(grp,OM_ADDMEMBER,o);
		    }
		    p+=strlen(p)+1;
		}
		if(!ok) {
		    MUI_DisposeObject(grp);
		    grp=NULL;
		}
	    }
	    dat->infowin=WindowObject,
		MUIA_Window_Title,name,
		MUIA_Window_ID,MAKE_ID('I','N','F','O'),
		MUIA_Window_IsSubWindow,TRUE,
		MUIA_Window_RootObject,grp,
		MUIA_Window_Screen,get_screen(dat->app),
		End;
	    MyFreeMem(str);
	    if(dat->infowin) {
		DoMethod3(dat->app,OM_ADDMEMBER,dat->infowin);
		DoMethod9(dat->infowin,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			 dat->infowin,3,MUIM_Set,MUIA_Window_Open,FALSE);
	    }
	}
    }
    if(dat->infowin)
	set(dat->infowin,MUIA_Window_Open,TRUE);
    return 0;
}

extern const char *zoomEntries[];

Static ULONG docOpenPrefsWindow(struct IClass *cl,Object *obj) {
    DocumentData *dat=INST_DATA(cl,obj);
    if(!dat->prefswin && dat->app) {
	Object *useobj,*saveobj;
	const char* name;
	static const char *titles[3];
	ULONG page,zoom,rotate,num_pages;
	struct PDFDoc *pdfdoc;
	Object *strobj,*reggrp;
	struct MYS_DocBitmap_CropBox cropbox;
	titles[0]=STR(MSG_DOCPREFS_GENERAL_PAGE);
	titles[1]=STR(MSG_DOCPREFS_FONTMAP_PAGE);
	get(obj,MYATTR_Document_Name,&name);
	get(obj,MYATTR_Document_OpenPage,&page);
	get(obj,MYATTR_Document_OpenZoom,&zoom);
	get(obj,MYATTR_Document_OpenRotate,&rotate);
	get(obj,MYATTR_Document_NumPages,&num_pages);
	get(obj,MYATTR_Document_PDFDoc,&pdfdoc);
	get(obj,MYATTR_Document_OpenCropBox,&cropbox);
	strobj=StringObject,
	    StringFrame,
	    MUIA_String_MaxLen,32,
	    MUIA_CycleChain,TRUE,
	    End;
	dat->prefswin=WindowObject,
	    MUIA_HelpNode,"docsettings",
	    MUIA_Window_Title,name,
	    MUIA_Window_ID,MAKE_ID('D','P','R','F'),
	    MUIA_Window_Screen,get_screen(dat->app),
	    MUIA_Window_IsSubWindow,TRUE,
	    MUIA_Window_RootObject,VGroup,
		Child,reggrp=RegisterObject,
		    MUIA_Register_Titles,titles,
		    Child,ColGroup(2),
			MUIA_HelpNode,"docprf_general",
			Child,Label1(STR(MSG_DOCPREFS_OPEN_PAGE_GAD)),
			Child,dat->pageprf=PageSliderObject,
			    MUIA_ShortHelp,STR(MSG_DOCPREFS_OPEN_PAGE_HELP),
			    MYATTR_PageSlider_Document,obj,
			    MYATTR_PageSlider_Value,page,
			    MYATTR_PageSlider_CommInfo,dat->comm_info,
			    End,
			Child,Label1(STR(MSG_DOCPREFS_ZOOM_GAD)),
			Child,dat->zoomprf=CycleObject,
			    MUIA_Cycle_Entries,zoomEntries,
			    MUIA_Cycle_Active,zoom,
			    MUIA_CycleChain,TRUE,
			    MUIA_ControlChar,'z',
			    MUIA_ShortHelp,STR(MSG_DOCPREFS_ZOOM_HELP),
			    End,
			Child,Label1(STR(MSG_DOCPREFS_ROTATE_GAD)),
			Child,dat->rotateprf=MyRotateSliderObject,
			    SliderFrame,
			    MUIA_Background,MUII_SliderBack,
			    MUIA_Slider_Horiz,TRUE,
			    MUIA_Numeric_Min,0,
			    MUIA_Numeric_Max,3,
			    MUIA_Numeric_Value,rotate/90,
			    MUIA_CycleChain,TRUE,
			    MUIA_ShortHelp,STR(MSG_DOCPREFS_ROTATE_HELP),
			    End,
			AA(Child,Label1(STR(MSG_DOCPREFS_ANTIALIASING_GAD)),)
			AA(Child,(HGroup,
			    MUIA_ShortHelp,STR(MSG_DOCPREFS_ANTIALIASING_HELP),
			    Child,dat->textaaprf=MUI_MakeObject(MUIO_Checkmark,STR(MSG_DOCPREFS_AA_TEXT_GAD)),
			    Child,LLabel1(STR(MSG_DOCPREFS_AA_TEXT_GAD)),
			    Child,dat->fillaaprf=MUI_MakeObject(MUIO_Checkmark,STR(MSG_DOCPREFS_AA_FILL_GAD)),
			    Child,LLabel1(STR(MSG_DOCPREFS_AA_FILL_GAD)),
			    Child,dat->strokeaaprf=MUI_MakeObject(MUIO_Checkmark,STR(MSG_DOCPREFS_AA_STROKE_GAD)),
			    Child,LLabel1(STR(MSG_DOCPREFS_AA_STROKE_GAD)),
			    Child,RectangleObject,
				End,
			    End),)
			Child,Label1(STR(MSG_DOCPREFS_CONTINUOUS_GAD)),
			Child,HGroup,
			    MUIA_ShortHelp,STR(MSG_DOCPREFS_CONTINUOUS_HELP),
			    Child,dat->continuousprf=MUI_MakeObject(MUIO_Checkmark,STR(MSG_DOCPREFS_CONTINUOUS_GAD)),
			    Child,RectangleObject,
				End,
			    End,
			Child,Label1(STR(MSG_DOCPREFS_OUTLINES_GAD)),
			Child,HGroup,
			    MUIA_ShortHelp,STR(MSG_DOCPREFS_OUTLINES_HELP),
			    Child,dat->outlinesprf=MUI_MakeObject(MUIO_Checkmark,STR(MSG_DOCPREFS_OUTLINES_GAD)),
			    Child,RectangleObject,
				End,
			    End,
			Child,Label1(STR(MSG_DOCPREFS_DISPLAY_MODE_GAD)),
			Child,dat->dispmodeprf=CycleObject,
			    MUIA_Cycle_Entries,dispEntries,
			    MUIA_Cycle_Active,dat->open_dispmode,
			    MUIA_CycleChain,TRUE,
			    MUIA_ShortHelp,STR(MSG_DOCPREFS_DISPLAY_MODE_HELP),
			    End,
			Child,Label1(STR(MSG_DOCPREFS_COLUMNS_GAD)),
			Child,dat->columnsprf=MUI_MakeObject(MUIO_Slider,STR(MSG_DOCPREFS_COLUMNS_GAD),1,10),
			Child,Label1(STR(MSG_PREFS_ROWS_GAD)),
			Child,dat->rowsprf=MUI_MakeObject(MUIO_Slider,STR(MSG_DOCPREFS_ROWS_GAD),1,10),
			Child,Label1(STR(MSG_DOCPREFS_TOOLBAR_GAD)),
			Child,HGroup,
			    MUIA_ShortHelp,STR(MSG_DOCPREFS_TOOLBAR_HELP),
			    Child,dat->toolbarprf=MUI_MakeObject(MUIO_Checkmark,STR(MSG_DOCPREFS_TOOLBAR_GAD)),
			    Child,RectangleObject,
				End,
			    End,
			Child,Label1(STR(MSG_DOCPREFS_NAVBAR_GAD)),
			Child,HGroup,
			    MUIA_ShortHelp,STR(MSG_DOCPREFS_NAVBAR_HELP),
			    Child,dat->navbarprf=MUI_MakeObject(MUIO_Checkmark,STR(MSG_DOCPREFS_NAVBAR_GAD)),
			    Child,RectangleObject,
				End,
			    End,
			Child,Label1(STR(MSG_DOCPREFS_CROPBOX_GAD)),
			Child,HGroup,
			    Child,ColGroup(4),
				MUIA_Weight,1000,
				MUIA_ShortHelp,STR(MSG_DOCPREFS_CROPBOX_HELP),
				Child,Label2(STR(MSG_DOCPREFS_CB_LEFT_GAD)),
				Child,dat->leftprf=StringObject,
				    StringFrame,
				    MUIA_String_Accept,"0123456789",
				    MUIA_String_AdvanceOnCR,TRUE,
				    MUIA_String_Integer,cropbox.left,
				    MUIA_String_MaxLen,8,
				    MUIA_CycleChain,TRUE,
				    End,
				Child,Label2(STR(MSG_DOCPREFS_CB_TOP_GAD)),
				Child,dat->topprf=StringObject,
				    StringFrame,
				    MUIA_String_Accept,"0123456789",
				    MUIA_String_AdvanceOnCR,TRUE,
				    MUIA_String_Integer,cropbox.top,
				    MUIA_String_MaxLen,8,
				    MUIA_CycleChain,TRUE,
				    End,
				Child,Label2(STR(MSG_DOCPREFS_CB_RIGHT_GAD)),
				Child,dat->rightprf=StringObject,
				    StringFrame,
				    MUIA_String_Accept,"0123456789",
				    MUIA_String_AdvanceOnCR,TRUE,
				    MUIA_String_Integer,cropbox.right,
				    MUIA_String_MaxLen,8,
				    MUIA_CycleChain,TRUE,
				    End,
				Child,Label2(STR(MSG_DOCPREFS_CB_BOTTOM_GAD)),
				Child,dat->bottomprf=StringObject,
				    StringFrame,
				    MUIA_String_Accept,"0123456789",
				    MUIA_String_AdvanceOnCR,TRUE,
				    MUIA_String_Integer,cropbox.bottom,
				    MUIA_String_MaxLen,8,
				    MUIA_CycleChain,TRUE,
				    End,
				End,
			    Child,BoxGrabberObject,
				ButtonFrame,
				MUIA_Background,MUII_ButtonBack,
				MYATTR_PageSet_Document,dat,
				MUIA_Text_Contents,STR(MSG_DOCPREFS_CB_GRAB_GAD),
				MUIA_Draggable,TRUE,
				MUIA_ShortHelp,STR(MSG_DOCPREFS_CB_GRAB_HELP),
				End,
			    End,
			End,
		    End,
		Child,HGroup,
		    Child,saveobj=SimpleButton(STR(MSG_SETTINGS_SAVE_GAD)),
		    Child,useobj=SimpleButton(STR(MSG_SETTINGS_USE_GAD)),
		    End,
		End,
	    End;
	if(dat->prefswin) {
	    /* we don't want dat->fontmap to be freed if prefswin fails
	     * to be created. So add it now. */
	    DoMethod3(reggrp,OM_ADDMEMBER,dat->fontmap);
	    set(dat->textaaprf,MUIA_Selected,dat->open_textaa);
	    set(dat->fillaaprf,MUIA_Selected,dat->open_fillaa);
	    set(dat->strokeaaprf,MUIA_Selected,dat->open_strokeaa);
	    set(dat->continuousprf,MUIA_Selected,dat->open_continuous);
	    set(dat->outlinesprf,MUIA_Selected,dat->open_outlines);
	    set(dat->columnsprf,MUIA_Numeric_Value,dat->open_columns);
	    set(dat->rowsprf,MUIA_Numeric_Value,dat->open_rows);
	    set(dat->toolbarprf,MUIA_Selected,dat->open_toolbar);
	    set(dat->navbarprf,MUIA_Selected,dat->open_navbar);
	    DoMethod9(dat->prefswin,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
		     dat->prefswin,3,MUIM_Set,MUIA_Window_Open,FALSE);
	    DoMethod7(useobj,MUIM_Notify,MUIA_Pressed,FALSE,
		     obj,1,MYM_Document_UsePrefs);
	    DoMethod9(useobj,MUIM_Notify,MUIA_Pressed,FALSE,
		     dat->prefswin,3,MUIM_Set,MUIA_Window_Open,FALSE);
	    DoMethod7(saveobj,MUIM_Notify,MUIA_Pressed,FALSE,
		     obj,1,MYM_Document_SavePrefs);
	    DoMethod9(saveobj,MUIM_Notify,MUIA_Pressed,FALSE,
		     dat->prefswin,3,MUIM_Set,MUIA_Window_Open,FALSE);
	    DoMethod3(dat->app,OM_ADDMEMBER,dat->prefswin);
	}
    }
    if(dat->prefswin)
	set(dat->prefswin,MUIA_Window_Open,TRUE);
    return 0;
}

Static ULONG docSavePrefs(struct IClass *cl,Object *obj) {
    DocumentData *dat=INST_DATA(cl,obj);
    BOOL ret=FALSE;
    LONG entries;
    struct DiskObject* dob;
    BPTR olddir;
    Object *o=dat->fontmap;
    BPTR dir;
    const char* name;
    get(obj,MYATTR_Document_Dir,&dir);
    get(obj,MYATTR_Document_Name,&name);
    get(o,MYATTR_FontMap_Entries,&entries);
    olddir=CurrentDir(dir);
    if((dob=GetDiskObject((char*)name)) ||
       (dob=GetDiskObject((char*)get_def_icon())) ||
       (dob=GetDiskObjectNew((char*)name))) {
	STRPTR* oldtt=dob->do_ToolTypes;
	LONG numoldtt=0;
	STRPTR* p;
	STRPTR* newtt;
	static const char *ttnames[]={
	    "FONTMAP","PAGE","ZOOM","ROTATE","MARGINS",
#if HAS_AA
	    "ANTIALIASING",
#endif
	    "COLUMNS","ROWS","CONTINUOUS","OUTLINES","DISPLAYMODE","TOOLBAR",
	    "NAVBAR"
	};
	const int numttnames=sizeof(ttnames)/sizeof(ttnames[0]);
	if(oldtt)
	    for(p=oldtt;*p;++p) {
		int k;
		for(k=0;k<numttnames;++k)
		    if(Strnicmp(*p,(char*)ttnames[k],strlen(ttnames[k])))
			break;
		if(k==numttnames)
		    ++numoldtt;
	    }

	if(newtt=MyAllocMem(sizeof(*newtt)*(numoldtt+entries+numttnames))) {
	    STRPTR* q=newtt;
	    int k;
	    BOOL err=FALSE;
	    ULONG x;

	    if(oldtt) {
		for(p=oldtt;*p;++p) {
		    int k;
		    for(k=0;k<numttnames;++k)
			if(Strnicmp(*p,(char*)ttnames[k],strlen(ttnames[k])))
			    break;
		    if(k==numttnames)
			*q++=*p;
		}
	    }

	    get(dat->pageprf,MYATTR_PageSlider_Value,&x);
	    if(x!=1) {
		static char buf[16];
		sprintf(buf,"PAGE=#%ld",x);
		*q++=buf;
	    }
	    {
		static char buf[16];
		char *t=buf+5;
		strcpy(buf,"ZOOM=");
		get(dat->zoomprf,MUIA_Cycle_Active,&x);
		if(x==zoomPage)
		    strcpy(t,"FITPAGE");
		else if(x==zoomWidth)
		    strcpy(t,"FITWIDTH");
		else {
		    if(x<5) {
			*t++='-';
			*t++='5'-x;
		    } else
			*t++='0'-5+x;
		    *t++='\0';
		}
		*q++=buf;
	    }
	    get(dat->rotateprf,MUIA_Numeric_Value,&x);
	    if(x) {
		static const char *rotatett[]={
		    "ROTATE=90","ROTATE=180","ROTATE=270"
		};
		*q++=(char*)rotatett[x-1];
	    }
	    {
		LONG x1,y1,x2,y2;
		static char buf[64];
		get(dat->leftprf,MUIA_String_Integer,&x1);
		get(dat->topprf,MUIA_String_Integer,&y1);
		get(dat->rightprf,MUIA_String_Integer,&x2);
		get(dat->bottomprf,MUIA_String_Integer,&y2);
		if(x1 || y1 || x2 || y2) {
		    sprintf(buf,"MARGINS=%ld/%ld/%ld/%ld",x1,y1,x2,y2);
		    *q++=buf;
		}
	    }
#if HAS_AA
	    {
		static char buf[32];
		size_t sz;
		strcpy(buf,"ANTIALIASING=");
		get(dat->textaaprf,MUIA_Selected,&x);
		if(x)
		    strcat(buf,"TEXT|");
		get(dat->fillaaprf,MUIA_Selected,&x);
		if(x)
		    strcat(buf,"FILL|");
		get(dat->strokeaaprf,MUIA_Selected,&x);
		if(x)
		    strcat(buf,"STROKE|");
		sz=strlen(buf)-1;
		if(buf[sz]=='|')
		    buf[sz]='\0';
		*q++=buf;
	    }
#endif
	    {
		static char buf[16];
		get(dat->columnsprf,MUIA_Numeric_Value,&x);
		sprintf(buf,"COLUMNS=%ld",x);
		*q++=buf;
	    }
	    {
		static char buf[16];
		get(dat->rowsprf,MUIA_Numeric_Value,&x);
		sprintf(buf,"ROWS=%ld",x);
		*q++=buf;
	    }
	    get(dat->continuousprf,MUIA_Selected,&x);
	    *q++=x?"CONTINUOUS=YES":"CONTINUOUS=NO";
	    get(dat->outlinesprf,MUIA_Selected,&x);
	    *q++=x?"OUTLINES=YES":"OUTLINES=NO";
	    {
		static char buf[32];
		static const char *entry[]={"ONEPASS","TEXTFIRST","PROGRESSIVE"};
		get(dat->dispmodeprf,MUIA_Cycle_Active,&x);
		sprintf(buf,"DISPLAYMODE=%s",entry[x]);
		*q++=buf;
	    }
	    get(dat->toolbarprf,MUIA_Selected,&x);
	    *q++=x?"TOOLBAR=YES":"TOOLBAR=NO";
	    get(dat->navbarprf,MUIA_Selected,&x);
	    *q++=x?"NAVBAR=YES":"NAVBAR=NO";
	    p=q;

	    for(k=0;k<entries;++k) {
		struct fontmapentry* e;
		size_t l1,l2;
		char* s;

		DoMethod4(o,MYM_FontMap_GetEntry,k,&e);
		l1=strlen(e->pdffont);
		l2=strlen(e->font);
		if(s=MyAllocMem(l1+l2+16)) {
		    *q++=s;
		    memcpy(s,"FONTMAP=",8);
		    s+=8;
		    memcpy(s,e->pdffont,l1);
		    s+=l1;
		    *s++='/';
		    memcpy(s,e->font,l2);
		    s+=l2;
		    *s++='[';
		    *s++='T';
		    *s++=e->type==4?'T':'1';
		    *s++=']';
		    *s='\0';
		} else
		    err=TRUE;
	    }
	    *q=NULL;

	    dob->do_Type=WBPROJECT;
	    dob->do_ToolTypes=newtt;
	    if(PutDiskObject((char*)name,dob) && !err)
		ret=TRUE;

	    while(*p)
		MyFreeMem(*p++);
	    MyFreeMem(newtt);
	}
	dob->do_ToolTypes=oldtt;
	FreeDiskObject(dob);
    }
    CurrentDir(olddir);
    if(!ret) {
	Object *app;
	get(obj,MUIA_ApplicationObject,&app);
	MUI_RequestA(app,obj,0,(char*)STR(MSG_ERROR_ERR),(char*)STR(MSG_REQ_OK),(char*)STR(MSG_CANT_SAVE_PREFS_ERR),NULL);
    }
    DoMethod2(obj,MYM_Document_UsePrefs);
    return ret;
}

Static ULONG docLoadPrefs(struct IClass *cl,Object *obj) {
    DocumentData *dat=INST_DATA(cl,obj);
    struct DiskObject *dob;
    BPTR olddir;
    BPTR dir;
    const char* name;
    get(obj,MYATTR_Document_Dir,&dir);
    get(obj,MYATTR_Document_Name,&name);
    olddir=CurrentDir(dir);
    if(dob=GetDiskObject((char*)name)) {
	char** p=(char**)dob->do_ToolTypes;
	if(p) {
	    char* s=(char*)FindToolType((UBYTE**)p,(UBYTE*)"PAGE");
	    if(s)
		set(obj,MYATTR_Document_OpenPageLabel,s);
	    s=(char*)FindToolType((UBYTE**)p,(UBYTE*)"ZOOM");
	    if(s) {
		int z;
		if(!Stricmp(s,"FITPAGE"))
		    z=zoomPage;
		else if(!Stricmp(s,"FITWIDTH"))
		    z=zoomWidth;
		else
		    z=atoi(s)+5;
		set(obj,MYATTR_Document_OpenZoom,z);
	    }
	    s=(char*)FindToolType((UBYTE**)p,(UBYTE*)"ROTATE");
	    if(s)
		set(obj,MYATTR_Document_OpenRotate,atoi(s));
#if HAS_AA
	    if(s=(char*)FindToolType((UBYTE**)p,(UBYTE*)"ANTIALIASING")) {
		dat->open_textaa=MatchToolValue((UBYTE*)s,(UBYTE*)"TEXT");
		dat->open_fillaa=MatchToolValue((UBYTE*)s,(UBYTE*)"FILL");
		dat->open_strokeaa=MatchToolValue((UBYTE*)s,(UBYTE*)"STROKE");
	    }
#endif
	    s=(char*)FindToolType((UBYTE**)p,(UBYTE*)"MARGINS");
	    if(s) {
		struct MYS_DocBitmap_CropBox box;
		box.left=atoi(s);
		if(box.left<0)
		    box.left=0;
		while(*s && *s++!='/');
		box.top=atoi(s);
		if(box.top<0)
		    box.top=0;
		while(*s && *s++!='/');
		box.right=atoi(s);
		if(box.right<0)
		    box.right=0;
		while(*s && *s++!='/');
		box.bottom=atoi(s);
		if(box.bottom<0)
		    box.bottom=0;
		set(obj,MYATTR_Document_OpenCropBox,&box);
	    }
	    s=(char*)FindToolType((UBYTE**)p,(UBYTE*)"COLUMNS");
	    if(s)
		dat->open_columns=atoi(s);
	    s=(char*)FindToolType((UBYTE**)p,(UBYTE*)"ROWS");
	    if(s)
		dat->open_rows=atoi(s);
	    s=(char*)FindToolType((UBYTE**)p,(UBYTE*)"CONTINUOUS");
	    if(s)
		dat->open_continuous=!Stricmp(s,"YES");
	    s=(char*)FindToolType((UBYTE**)p,(UBYTE*)"OUTLINES");
	    if(s)
		dat->open_outlines=!Stricmp(s,"YES");
	    s=(char*)FindToolType((UBYTE**)p,(UBYTE*)"DISPLAYMODE");
	    if(s) {
		if(!Stricmp(s,"ONEPASS"))
		    dat->open_dispmode=DrAllAtOnce;
		else if(!stricmp(s,"TEXTFIRST"))
		    dat->open_dispmode=DrTextFirst;
		else
		    dat->open_dispmode=DrPartialUpdates;
	    }
	    s=(char*)FindToolType((UBYTE**)p,(UBYTE*)"TOOLBAR");
	    if(s)
		dat->open_toolbar=!Stricmp(s,"YES");
	    s=(char*)FindToolType((UBYTE**)p,(UBYTE*)"NAVBAR");
	    if(s)
		dat->open_navbar=!Stricmp(s,"YES");
	    if(dat->fontmap) {
		while(*p) {
		    if(!Strnicmp(*p,"FONTMAP=",8)) {
			const char *q=*p+8;
			struct fontmapentry e;
			const char *p=q;
			const char *r;
			while(*p && *p!='/')
			    ++p;
			if(*p=='/' && p!=q && p-q<sizeof(e.pdffont)) {
			    memcpy(e.pdffont,q,p-q);
			    e.pdffont[p-q]='\0';
			    if(*++p=='[') {
				//printf("Invalid fontmap entry: \"%s\".\n",q);
				break;
			    }
			    q=p;
			    while(*p && *p!='[')
				++p;
			    if(*p=='[' && p!=q && p-q<sizeof(e.font)) {
				memcpy(e.font,q,p-q);
				e.font[p-q]='\0';
				if(*++p=='T' && (*++p=='1' || *p=='T') &&
				   *++p==']') {
				    e.type=p[-1]=='1'?1:4;
				    DoMethod4(dat->fontmap,MUIM_List_InsertSingle,&e,MUIV_List_Insert_Bottom);
				}
			    }
			} /*else
			printf("Invalid fontmap entry: \"%s\".\n",q);*/
		    }
		    ++p;
		}
	    }
	}
	FreeDiskObject(dob);
    }
    CurrentDir(olddir);
    return TRUE;
}

Static ULONG docUsePrefs(struct IClass *cl,Object *obj) {
    DocumentData *dat=INST_DATA(cl,obj);
    LONG entries;
    int k;
    clear_fontmap(dat->comm_info,dat->pdfdoc);
    if(dat->fontmap) {
	get(dat->fontmap,MYATTR_FontMap_Entries,&entries);
	for(k=0;k<entries;++k) {
	    struct fontmapentry* e;
	    DoMethod4(dat->fontmap,MYM_FontMap_GetEntry,k,&e);
	    add_fontmap(dat->comm_info,dat->pdfdoc,e->pdffont,e->font,e->type);
	}
    }
    DoMethod2(obj,MYM_Document_RefreshWindows);
    return 0;
}

Static ULONG docOpenScanWindow(struct IClass *cl,Object *obj) {
    DocumentData *dat=INST_DATA(cl,obj);
    if(!dat->scanwin && dat->fontmap) {
	Object *scanobj,*removeobj,*clearobj;
	int num_pages;
	get(obj,MYATTR_Document_NumPages,&num_pages);
	dat->scanwin=WindowObject,
	    MUIA_HelpNode,"scanfonts",
	    MUIA_Window_Title,STR(MSG_FONTS_WIN),
	    MUIA_Window_ID,MAKE_ID('S','C','A','N'),
	    MUIA_Window_Screen,get_screen(dat->app),
	    MUIA_Window_IsSubWindow,TRUE,
	    WindowContents,VGroup,
		Child,dat->scanlv=FontInfoLVObject,
		    MYATTR_FontInfoLV_CommInfo,dat->comm_info,
		    End,
		Child,ColGroup(2),
		    Child,Label2(STR(MSG_FIRST_PAGE_GAD)),
		    Child,dat->scanstartobj=PageSliderObject,
			MYATTR_PageSlider_Document,obj,
			MYATTR_PageSlider_CommInfo,dat->comm_info,
			MYATTR_PageSlider_Value,1,
			End,
		    Child,Label2(STR(MSG_LAST_PAGE_GAD)),
		    Child,dat->scanstopobj=PageSliderObject,
			MYATTR_PageSlider_Document,obj,
			MYATTR_PageSlider_CommInfo,dat->comm_info,
			MYATTR_PageSlider_Value,1,
			End,
		    End,
		Child,HGroup,
		    Child,removeobj=SimpleButton(STR(MSG_FONTS_REMOVE)),
		    Child,clearobj=SimpleButton(STR(MSG_FONTS_CLEAR)),
		    Child,scanobj=SimpleButton(STR(MSG_FONTS_SCAN)),
		    End,
		End,
	    End;

	if(dat->scanwin) {
	    DoMethod9(dat->scanstartobj,MUIM_Notify,MYATTR_PageSlider_Value,MUIV_EveryTime,
		     dat->scanstartobj,3,MUIM_CallHook,&start_hook,dat->scanstopobj);
	    DoMethod9(dat->scanstopobj,MUIM_Notify,MYATTR_PageSlider_Value,MUIV_EveryTime,
		     dat->scanstopobj,3,MUIM_CallHook,&stop_hook,dat->scanstartobj);
	    DoMethod7(scanobj,MUIM_Notify,MUIA_Pressed,FALSE,
		     obj,1,MYM_Document_Scan);
	    DoMethod7(clearobj,MUIM_Notify,MUIA_Pressed,FALSE,
		     dat->scanlv,1,MUIM_List_Clear);
	    DoMethod8(removeobj,MUIM_Notify,MUIA_Pressed,FALSE,
		     dat->scanlv,2,MUIM_List_Remove,MUIV_List_Remove_Selected);
	    DoMethod3(dat->app,OM_ADDMEMBER,dat->scanwin);
	    DoMethod9(dat->scanwin,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
		     dat->scanwin,3,MUIM_Set,MUIA_Window_Open,FALSE);
	}
    }
    if(dat->scanwin)
	set(dat->scanwin,MUIA_Window_Open,TRUE);
    return 0;
}


Static ULONG docScan(struct IClass *cl,Object *obj) {
    DocumentData *dat=INST_DATA(cl,obj);
    int first_page;
    int last_page;
    get(dat->scanstartobj,MYATTR_PageSlider_Value,&first_page);
    get(dat->scanstopobj,MYATTR_PageSlider_Value,&last_page);
    if(last_page>=first_page) {
	int k,l=0;
	ULONG sigs=0;
	Object *gauge;
	struct FontOutputDev *out;
	gauge=create_gauge(dat->app,STR(MSG_FONTS_SCANNING),last_page-first_page+1);
	if(out=init_scan(dat->comm_info)) {
	    LONG r;
	    int num=0;
	    for(k=first_page;k<=last_page;++k) {
		num=scan_fonts(dat->comm_info,dat->pdfdoc,out,k,k+1);
		if(gauge) {
		    set(gauge,MUIA_Gauge_Current,++l);
		    r=DoMethod3(dat->app,MUIM_Application_NewInput,&sigs);
		    if(r==MUIV_Application_ReturnID_Quit ||
		       r==ID_ABORT)
			break;
		}
	    }
	    DoMethod4(dat->scanlv,MYM_FontInfoLV_AddDevContents,out,num);
	    end_scan_fonts(dat->comm_info,out);
	}
	delete_gauge(gauge);
    } else
	DisplayBeep(NULL);
    return 0;
}

Static ULONG docRefreshWindows(struct IClass *cl,Object *obj) {
    DocumentData *dat=INST_DATA(cl,obj);
    struct List *list;
    APTR state;
    Object *w;
    get(dat->app,MUIA_Application_WindowList,&list);
    state=list->lh_Head;
    while(w=NextObject(&state)) {
	Object *doc;
	if(get(w,MYATTR_DocWindow_Document,&doc) && doc==obj) {
	    Object *o;
	    get(w,MUIA_Window_RootObject,&o);
	    /* Use PushMethod as refresh may close/open subwindows */
	    DoMethod5(dat->app,MUIM_Application_PushMethod,o,1,MYM_DocGroup_Refresh);
	}
    }
    return 0;
}

BEGIN_DISPATCHER(docDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:                       return docNew      (cl,obj,(APTR)msg);
      case OM_DISPOSE:                   return docDispose  (cl,obj,msg);
      case OM_GET:                       return docGet      (cl,obj,(APTR)msg);
      /*case OM_SET:                       return docSet      (cl,obj,(APTR)msg);*/
      case MYM_Document_IncRef:          return docIncRef   (cl,obj);
      case MYM_Document_DecRef:          return docDecRef   (cl,obj);
      case MYM_Document_OpenInfoWindow:  return docOpenInfoWindow (cl,obj);
      case MYM_Document_OpenPrefsWindow: return docOpenPrefsWindow(cl,obj);
      case MYM_Document_OpenScanWindow:  return docOpenScanWindow (cl,obj);
      case MYM_Document_SavePrefs:       return docSavePrefs(cl,obj);
      case MYM_Document_LoadPrefs:       return docLoadPrefs(cl,obj);
      case MYM_Document_UsePrefs:        return docUsePrefs (cl,obj);
      case MYM_Document_Scan:            return docScan     (cl,obj);
      case MYM_Document_RefreshWindows:  return docRefreshWindows(cl,obj);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(docDispatcher)



struct MUI_CustomClass *document_mcc;
static int count;

BOOL createDocumentClass(void) {
    if(count++==0) {
	if(createSimpleDocClass()) {
	    if(boxgrabber_mcc=MUI_CreateCustomClass(NULL,MUIC_Text,NULL,sizeof(BoxGrabberData),(APTR)&bgDispatcher)) {
		if(document_mcc=MUI_CreateCustomClass(NULL,NULL,simpledoc_mcc,sizeof(DocumentData),(APTR)&docDispatcher))
		    return TRUE;
		MUI_DeleteCustomClass(boxgrabber_mcc);
	    }
	    deleteSimpleDocClass();
	}
	count=0;
	return FALSE;
    }
    return TRUE;
}

BOOL deleteDocumentClass(void) {
    if(count && --count==0) {
	MUI_DeleteCustomClass(document_mcc);
	MUI_DeleteCustomClass(boxgrabber_mcc);
	deleteSimpleDocClass();
    }
    return TRUE;
}

