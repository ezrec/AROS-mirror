/*************************************************************************\
 *                                                                       *
 * Vpdf.c                                                                *
 *                                                                       *
 * Copyright 1999-2002 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <libraries/mui.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <proto/muimaster.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/iffparse.h>
#include <proto/locale.h>
#include "AComm.h"
#include "Apdf.h"
#include "document.h"
#include "docbitmap.h"
#include "fontmap.h"
#include "docgroup.h"
#include "prefs.h"
#include "comm.h"
#include "msgs.h"
#include "io_client.h"

#ifdef HAVE_POWERUP
struct ModuleArgs;
struct PPCObjectInfo;
#   include <powerup/ppclib/ppc.h>
#   include <powerup/ppclib/message.h>
#   include <powerup/ppclib/tasks.h>
struct ModuleArgs;    /* avoid warnings */
struct PPCObjectInfo; /* avoid warnings */
#   undef NO_INLINE_STDARG
#   ifdef MORPHOS
#       include <proto/ppc.h>
#   else
#       include <powerup/proto/ppc.h>
#   endif
#endif

#ifndef AFF_68060
#   define AFF_68060 (1L<<7)
#endif

#define BUILDPLUGIN
#include "v_plugin.h"

#define Static  static
#define DEBUG   0

#if DEBUG
#   define DB(x)   if(file) { x }
#else
#   define DB(x)
#endif

struct ExecBase *SysBase;
struct DosLibrary *DOSBase;
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *UtilityBase;
struct Library *LocaleBase;
struct Library* MUIMasterBase;
struct Library* PPCLibBase;
struct Library* IFFParseBase;
struct Library* KeymapBase;
struct Library* LayersBase;
struct Library* CyberGfxBase;
#if DEBUG
BPTR file;
#endif

//struct Semaphore mySemaphore;

struct vplug_functable *functable;

struct MUI_CustomClass *mcc;
Object *config;
const char *module;
void* pool;
BPTR modulesdir;
struct Catalog *catalog;
BPTR console_in,console_out;
BOOL is38;

#ifdef HAVE_POWERUP
void* elfobject;
#endif
BPTR seglist;
static IoServer io_server;

struct server_info sv;
struct comm_info *ci;

/*void lock(void) {
    ObtainSemaphore(&mySemaphore);
}

void unlock(void) {
    ReleaseSemaphore(&mySemaphore);
}*/

#ifdef MORPHOS

ULONG myDoMethod(void *obj,ULONG a1,ULONG a2,ULONG a3,ULONG a4,ULONG a5,ULONG a6,ULONG a7,ULONG a8,ULONG a9,ULONG a10) {
    ULONG t[10];
    t[0]=a1;
    t[1]=a2;
    t[2]=a3;
    t[3]=a4;
    t[4]=a5;
    t[5]=a6;
    t[6]=a7;
    t[7]=a8;
    t[8]=a9;
    t[9]=a10;
    return DoMethodA(obj,&t);
}

#endif

void* MyAllocMem(size_t sz) {
    char* p;
    LOCK;
    p=AllocPooled(pool,sz+=sizeof(size_t));
    UNLOCK;
    if(p) {
	*(size_t*)p=sz;
	p+=sizeof(size_t);
    }        /****errormsg*/
    return p;
}

void MyFreeMem(void* p) {
    if(p) {
	p=(size_t*)p-1;
	LOCK;
	FreePooled(pool,p,*(size_t*)p);
	UNLOCK;
    }
}

void CopyStr(char** p,const char* q) {
    if(q) {
	size_t l=strlen(q)+1;
	char* t=MyAllocMem(l);
	if(t) {
	    MyFreeMem(*p);
	    *p=t;
	    memcpy(t,q,l);
	}
    }
}

Object *getDocument(struct comm_info *ci,Object* app,Object* win,BPTR lock,const char* name) {
    return NULL;
}

void refresh_doc_fontmaps(void) {}

#define MYM_Embeded_Run     (MYTAG_START+1301)
#define MYM_Embeded_URL     (MYTAG_START+1302)
#define MYM_Embeded_Open    (MYTAG_START+1303)
#define MYM_Embeded_OpenNew (MYTAG_START+1304)
#define MYM_Embeded_Abort   (MYTAG_START+1305)

Static int initsv(struct comm_info *ci,int version,IoServer io_server) {
    struct msg_init* p=ci->cmddata;
    p->version=version;
    p->errors=0;
    p->abort_flag_ptr=sv.abort_flag_ptr;
    p->draw_state_ptr=sv.draw_state_ptr;
    p->io_server = io_server;
    return (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_INIT);
}

Static char *get_version(struct comm_info *ci) {
    struct msg_version *p=ci->cmddata;
    sync();
    if((*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_VERSION))
	return p->version;
    return "???";
}

struct EmbedData {
    Object *contents;
    Object *gauge;
    Object *abort;
    Object *comm;
    APTR stream;
    struct comm_info *ci;
    char filename[128];
    char gaugebuf[32];
    BOOL memlocked;
};
typedef struct EmbedData EmbedData;

static void my_vsprintf(char* buf,const char *fmt,void *args) {
    static const UWORD code[]={0x16c0,0x4e75};
    RawDoFmt((char*)fmt,args,(void(*)())code,buf);
}

Static ULONG eNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    /*char *url=(char*)GetTagData(VPLUG_EmbedInfo_URL,(ULONG)"Unknown",msg->ops_AttrList);*/
    APTR stream=(APTR)GetTagData(VPLUG_EmbedInfo_NetStream,0,msg->ops_AttrList);
    struct TagItem tags[4];
    Object *contents,*gauge,*abort;
    struct comm_info *ci;
    DB(FPrintf(file,"eNew(%lx,%s)\n",stream,GetTagData(VPLUG_EmbedInfo_URL,(ULONG)"Unknown",msg->ops_AttrList));)
    if(stream)
	ci=(*sv.create_comm_info)(&sv,sizeof(union msg_max));
    if(ci) {
	tags[0].ti_Tag=MUIA_Group_Child;
	tags[0].ti_Data=(ULONG)(contents=VGroup,
	    Child,RectangleObject,
		End,
	    Child,HGroup,
		Child,RectangleObject,
		    End,
	    Child,gauge=GaugeObject,
		GaugeFrame,
		MUIA_Gauge_Horiz,TRUE,
		MUIA_Gauge_InfoText,STR(MSG_LOADING_PLOBJ),
		    MUIA_Weight,900,
		    End,
		Child,RectangleObject,
		    End,
		End,
	    Child,HGroup,
		Child,RectangleObject,
		    End,
		Child,abort=SimpleButton(STR(MSG_ABORT_GAD)),
		Child,RectangleObject,
		    End,
		End,
	    Child,RectangleObject,
		End,
	    End);
	tags[1].ti_Tag=MUIA_Frame;
	tags[1].ti_Data=MUIV_Frame_Group;
	tags[2].ti_Tag=MUIA_Background;
	tags[2].ti_Data=MUII_GroupBack;
	tags[3].ti_Tag=TAG_MORE;
	tags[3].ti_Data=(ULONG)msg->ops_AttrList;
	msg->ops_AttrList=tags;
	if(obj=(Object *)DoSuperMethodA(cl,obj,(Msg)msg)) {
	    char *p;
	    ULONG x;
	    int len,cachesz;
	    EmbedData *dat=INST_DATA(cl,obj);
	    DB(FPrintf(file,"done supermethod\n");)
	    dat->contents=contents;
	    dat->gauge=gauge;
	    dat->abort=abort;
	    dat->stream=stream;
	    dat->ci=ci;
	    dat->memlocked=FALSE;
	    dat->comm=NULL;
	    len=VPLUG_NET_GETDOCLEN(functable,stream);
	    x=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_Cache);
	    cachesz=x?((struct CacheInfo *)x)->size:256;
	    cachesz*=1024;
	    DB(FPrintf(file,"len=%ld, cache=%ld\n",len,cachesz);)
	    if(len>=1024) {
		int arg=len>>10;
		my_vsprintf(dat->gaugebuf,"%%ldk of %ldk",&arg);
		SetAttrs(dat->gauge,MUIA_Gauge_Max,len>>10,
			 MUIA_Gauge_InfoText,dat->gaugebuf,TAG_END);
	    }
	    if(
#if defined(HAVE_POWERUP) && !defined(__MORPHOS__)
	       !elfobject && /* don't use settomem with PowerUp, as we know nothing
				about buffer alignment. This doesn't matter for MorphOS */
#endif
	       len>0 && (cachesz==0 || len<=cachesz)) {
		dat->filename[0]='\001';
		dat->filename[1]='\0';
		VPLUG_NET_SETTOMEM(functable,stream);
	    } else {
		Object *arg=obj;
		x=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_TmpDir);
		strncpy(dat->filename,x?(const char*)x:"t:",sizeof(dat->filename)-20);
		dat->filename[sizeof(dat->filename)-20]='\0';
		AddPart(dat->filename,"vpdf_",sizeof(dat->filename));
		p=dat->filename;
		while(*p++);
		--p;
		my_vsprintf(p,"%lx",&arg);
		VPLUG_NET_SETTOFILE(functable,stream,dat->filename,0);
	    }
	    DoMethod7(abort,MUIM_Notify,MUIA_Pressed,FALSE,
		      obj,1,MYM_Embeded_Abort);
	}
	msg->ops_AttrList=(struct TagItem*)tags[3].ti_Data;
    } else
	obj=NULL;
    DB(FPrintf(file,"end_new: %lx\n",obj);)
    return (ULONG)obj;
}

Static ULONG eDispose(struct IClass *cl,Object *obj,Msg msg) {
    EmbedData *dat=INST_DATA(cl,obj);
    if(dat->contents) {
	DoMethod3(obj,OM_REMMEMBER,dat->contents);
	MUI_DisposeObject(dat->contents);
    }
    if(dat->comm)
	cleanupComm();
    if(dat->filename[0]!='\001')
	DeleteFile(dat->filename);
    if(dat->memlocked)
	VPLUG_NET_UNLOCKDOCMEM(functable);
    (*sv.delete_comm_info)(dat->ci);
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG eFileInfo(struct IClass *cl,Object *obj) {
    EmbedData *dat=INST_DATA(cl,obj);
    int state;
    int len=VPLUG_NET_GETDOCLEN(functable,dat->stream);
    int cur=VPLUG_NET_GETDOCPTR(functable,dat->stream);
    DB(state=VPLUG_NET_STATE(functable,dat->stream);)
    DB(FPrintf(file,"FileInfo(%lx,%ld,%ld,%ld)\n",(LONG)obj,(LONG)state,len,cur);)
    set(dat->gauge,MUIA_Gauge_Current,cur>>10);
    return 0;
}

Static ULONG eFileData(struct IClass *cl,Object *obj) {
    EmbedData *dat=INST_DATA(cl,obj);
    int state;
    int len=VPLUG_NET_GETDOCLEN(functable,dat->stream);
    int cur=VPLUG_NET_GETDOCPTR(functable,dat->stream);
    DB(state=VPLUG_NET_STATE(functable,dat->stream);)
    DB(FPrintf(file,"FileData(%lx,%ld,%ld,%ld)\n",(LONG)obj,(LONG)state,len,cur);)
    set(dat->gauge,MUIA_Gauge_Current,cur>>10);
    return 0;
}

Static ULONG eFileDone(struct IClass *cl,Object *obj) {
    EmbedData *dat=INST_DATA(cl,obj);
    int state=VPLUG_NET_STATE(functable,dat->stream);
    Object *doc=NULL;
    DB(FPrintf(file,"FileDone(%lx,%ld)\n",(LONG)obj,(LONG)state);)
    if(state==VNS_DONE) {
	BPTR dir=0;
	char *name=dat->filename;
	Object *app;
	if(name[0]!='\001') {
	    ULONG x=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_TmpDir);
	    dir=Lock(x?(char *)x:"t:",ACCESS_READ);
	    name=FilePart(name);
	} else {
	    char *t=name+1;
	    unsigned x;
	    dat->memlocked=TRUE;
	    VPLUG_NET_LOCKDOCMEM(functable);
	    *t++=':';
	    *t++=':';
	    x=(unsigned)VPLUG_NET_GETDOCMEM(functable,dat->stream);
	    while(x) {
		*t++=(char)('0'+(x&7));
		x>>=3;
	    }
	    *t++=':';
	    x=VPLUG_NET_GETDOCPTR(functable,dat->stream);
	    while(x) {
		*t++=(char)('0'+(x&7));
		x>>=3;
	    }
	    *t='\0';
	}
	DB(FPrintf(file,"loading \"%s\"\n",(long)name);)
	get(obj,MUIA_ApplicationObject,&app);
	dat->comm=initComm(app);
	if(dat->comm) {
	    doc=VDocGroupObject,
		MYATTR_DocGroup_CommInfo,dat->ci,
		MYATTR_DocGroup_Document,SimpleDocObject,
		    MYATTR_Document_Dir,dir,
		    MYATTR_Document_Name,name,
		    MYATTR_Document_CommInfo,dat->ci,
		    MYATTR_Document_Application,app,
		    End,
		End;
	    if(doc) {
		DoMethod8(doc,MUIM_Notify,MYATTR_DocGroup_Run,MUIV_EveryTime,
			  obj,2,MYM_Embeded_Run,MUIV_TriggerValue);
		DoMethod8(doc,MUIM_Notify,MYATTR_DocGroup_URL,MUIV_EveryTime,
			  obj,2,MYM_Embeded_URL,MUIV_TriggerValue);
		DoMethod8(doc,MUIM_Notify,MYATTR_DocGroup_Open,MUIV_EveryTime,
			  obj,2,MYM_Embeded_Open,MUIV_TriggerValue);
		DoMethod8(doc,MUIM_Notify,MYATTR_DocGroup_OpenNew,MUIV_EveryTime,
			  obj,2,MYM_Embeded_OpenNew,MUIV_TriggerValue);
	    }
	}
	UnLock(dir);
    }
    if(!doc)
	doc=TextObject,
	    TextFrame,
	    MUIA_Text_Contents,state==VNS_FAILED?VPLUG_NET_ERRORSTRING(functable,dat->stream):(STRPTR)STR(MSG_LOAD_ERR),
	    MUIA_Text_SetVMax,FALSE,
	    MUIA_Text_PreParse,"\33c",
	    End;
    if(doc) {
	if(DoMethod2(obj,MUIM_Group_InitChange)) {
	    DoMethod3(obj,OM_REMMEMBER,dat->contents);
	    DoMethod3(obj,OM_ADDMEMBER,doc);
	    MUI_DisposeObject(dat->contents);
	    dat->contents=doc;
	    DoMethod2(obj,MUIM_Group_ExitChange);
	}
    }
    dat->gauge=NULL;
    dat->abort=NULL;
    DB(FPrintf(file,"Done.");)
    return 0;
}

Static ULONG eAbort(struct IClass *cl,Object *obj) {
    EmbedData *dat=INST_DATA(cl,obj);
    if(dat->stream) {
	int state=VPLUG_NET_STATE(functable,dat->stream);
	if(state==VNS_INPROGRESS)
	    (functable->vplug_net_abort)(dat->stream);
    }
    return 0;
}

struct MYP_Embeded_Run {
    ULONG MethodID;
    const char *name;
};

Static ULONG eRun(struct IClass *cl,Object *obj,struct MYP_Embeded_Run *msg) {
    return 0; /*****/
}

Static ULONG eURL(struct IClass *cl,Object *obj,struct MYP_Embeded_Run *msg) {
    VPLUG_SETURL(functable,(char*)msg->name,"",0);
    return 0;
}

Static ULONG eOpen(struct IClass *cl,Object *obj,struct MYP_Embeded_Run *msg) {
    return 0; /*****/
}

Static ULONG eOpenNew(struct IClass *cl,Object *obj,struct MYP_Embeded_Run *msg) {
    return 0; /*****/
}

BEGIN_DISPATCHER(eDispatcher,cl,obj,msg) {
#if DEBUG
    switch(msg->MethodID) {
      case OM_NEW:
      case OM_SET:
      case OM_GET:
      case MUIM_HandleInput:
	break;
      default: {
	  EmbedData *dat=INST_DATA(cl,obj);
	  if(file)
	      FPrintf(file,"Method=%lx\n",msg->MethodID);
	  break;
	}
    }
#endif
    switch(msg->MethodID) {
      case OM_NEW:                  return eNew     (cl,obj,(APTR)msg);
      case OM_DISPOSE:              return eDispose (cl,obj,msg);
      /*case OM_GET:                 return dwGet    (cl,obj,(APTR)msg);*/
      /*case OM_SET:                 return dwSet    (cl,obj,(APTR)msg);*/
      case MYM_Embeded_Run:         return eRun     (cl,obj,(APTR)msg);
      case MYM_Embeded_URL:         return eURL     (cl,obj,(APTR)msg);
      case MYM_Embeded_Open:        return eOpen    (cl,obj,(APTR)msg);
      case MYM_Embeded_OpenNew:     return eOpenNew (cl,obj,(APTR)msg);
      case MYM_Embeded_Abort:       return eAbort   (cl,obj);
      case VPLUG_NetStream_GotInfo: return eFileInfo(cl,obj);
      case VPLUG_NetStream_GotData: return eFileData(cl,obj);
      case VPLUG_NetStream_GotDone: return eFileDone(cl,obj);
	break;
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(eDispatcher)

BOOL cleanup(void) {
    if(mcc && MUI_DeleteCustomClass(mcc))
	mcc=NULL;
    if(mcc!=NULL || !deleteDocBitmapClass() ||
       !deleteVDocGroupClass() || !deleteSimpleDocClass() ||
       !deleteDocLensClass())
	return FALSE;

    if(config) {
	MUI_DisposeObject(config);
	config=NULL;
    }

    if(ci) {
	(*sv.delete_comm_info)(ci);
	ci=NULL;
	(*sv.destroy)(&sv);
    }
    cleanupIo(io_server);

    if(seglist) {
	UnLoadSeg(seglist);
	seglist=0;
    }
#ifdef HAVE_POWERUP
    if(elfobject) {
	PPCUnLoadObject(elfobject);
	elfobject=NULL;
    }
    CloseLibrary(PPCLibBase);
    PPCLibBase=NULL;
#endif
    module=NULL;
    if(modulesdir) {
	UnLock(modulesdir);
	modulesdir=0;
    }

    if(console_in) {
	Close(console_in);
	console_in=0;
    }

    if(console_out) {
	Close(console_out);
	console_out=0;
    }

    DeletePool(pool);
    pool=NULL;

#if DEBUG
    if(file) {
	Close(file);
	file=NULL;
    }
#endif

    CloseLibrary(CyberGfxBase);
    CyberGfxBase=NULL;
    CloseLibrary(MUIMasterBase);
    MUIMasterBase=NULL;
    CloseLibrary(KeymapBase);
    KeymapBase=NULL;
    CloseLibrary(IFFParseBase);
    IFFParseBase=NULL;
    CloseLibrary(LayersBase);
    LayersBase=NULL;
    CloseLibrary((struct Library*)UtilityBase);
    UtilityBase=NULL;
    CloseLibrary((struct Library*)GfxBase);
    GfxBase=NULL;
    CloseLibrary((struct Library*)IntuitionBase);
    IntuitionBase=NULL;
    CloseLibrary((struct Library*)DOSBase);
    DOSBase=NULL;
    if(catalog) {
	CloseCatalog(catalog);
	catalog=NULL;
    }
    CloseLibrary((struct Library*)LocaleBase);
    LocaleBase=NULL;
    return TRUE;
}

BOOL init(void) {
    SysBase=*(struct ExecBase **)4;
    LocaleBase=OpenLibrary("locale.library",0);
    DOSBase=(struct DosLibrary*)OpenLibrary("dos.library",39);
#if DEBUG
    if(DOSBase)
	file=Open("KCON:1/1/500/300/Vpdf/AUTO/WAIT/CLOSE/KEEPCLOSED",MODE_NEWFILE);
#endif
    IntuitionBase=(struct IntuitionBase*)OpenLibrary("intuition.library",39);
    UtilityBase=OpenLibrary("utility.library",39);
    MUIMasterBase=OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN);
    GfxBase=(struct GfxBase*)OpenLibrary("graphics.library",39);
    IFFParseBase=OpenLibrary("iffparse.library",39);
    KeymapBase=OpenLibrary("keymap.library",0);
    LayersBase=OpenLibrary("layers.library",36);
    if(DOSBase!=NULL && MUIMasterBase!=NULL &&
       IntuitionBase!=NULL && UtilityBase!=NULL && LayersBase!=NULL &&
       GfxBase!=NULL && IFFParseBase!=NULL && KeymapBase!=NULL) {
	if(pool=CreatePool(MEMF_ANY,50000,10000))
	    return TRUE;
    }
    is38=MUIMasterBase->lib_Version<=19;
    cleanup();
    return FALSE;
}

void set_table(struct vplug_functable *table) {
    functable=table;
}

void get_config(void) {
    if(!config) {
	if(config=MUI_NewObjectA(MUIC_Dataspace,NULL)) {
	    Object *pend=MUI_NewObjectA(MUIC_Pendisplay,NULL);
	    if(pend) {
		struct MUI_PenSpec *spec;
		DoMethod5(pend,MUIM_Pendisplay_SetRGB,0xffffffff,0,0);
		get(pend,MUIA_Pendisplay_Spec,&spec);
		MUI_DisposeObject(pend);
		if(!DoMethod5(config,MUIM_Dataspace_Add,spec,sizeof(*spec),MYCFG_Apdf_SelectColor)) {
		    MUI_DisposeObject(config);
		    config=NULL;
		}
	    }
	}
	DB(FPrintf(file,"config=%lx\n",(long)config);)
    }
}

void create_prefs(struct vplug_prefs *prefs) {
    DB(FPrintf(file,"create_prefs\n");)
    prefs->object=NULL;
    get_config();
    if(config && createPrefsClass()) {
	Object *txt;
	if(ci) {
	    const char *fmt;
	    txt=ListviewObject,
		MUIA_Listview_List,FloattextObject,
		    ReadListFrame,
		    MUIA_List_Format,"P="MUIX_C,
		    End,
		End;
	    fmt="Vpdf "vpdfVersion
#ifdef MORPHOS
		" (MorphOS version)"
#endif
		"\n%s \"%s\"\n\n"
		"%s";
		DoMethod7(txt,MUIM_SetAsString,MUIA_Floattext_Text,
			 fmt,STR(MSG_USED_MODULE),module,get_version(ci));
	} else {
	    txt=TextObject,
		MUIA_Text_Contents,MUIX_C "Vpdf "vpdfVersion
#ifdef MORPHOS
				   " (MorphOS version)"
#endif
				   "\n© 1999-2002 Emmanuel Lesueur\n\n"
				   "No module loaded.",
		MUIA_Text_SetVMax,FALSE,
		End;
	}
	prefs->object=PrefsObject,
	    MYATTR_Prefs_Plugin,TRUE,
	    MYATTR_Prefs_Page,txt,
	    End;
	if(prefs->object)
	    DoMethod3(prefs->object,MUIM_Settingsgroup_ConfigToGadgets,config);
	else
	    deletePrefsClass();
    }
    DB(if(!prefs->object) FPrintf(file,"...failed\n");)
}

void delete_prefs(struct vplug_prefs *prefs) {
    DB(FPrintf(file,"delete_prefs\n");)
    if(prefs->object) {
	MUI_DisposeObject(prefs->object);
	prefs->object=NULL;
	deletePrefsClass();
    }
}

void do_use_prefs(struct vplug_prefs *prefs) {
    DB(FPrintf(file,"use_prefs\n");)
    if(config && prefs->object) {
	DoMethod3(prefs->object,MUIM_Settingsgroup_GadgetsToConfig,config);
	if(ci)
	    use_prefs(ci,config);
    }
}

void do_load_prefs(struct vplug_prefs *prefs) {
    DB(FPrintf(file,"load_prefs\n");)
    get_config();
    if(config) {
	load_prefs(config,VPLUGPREFSPATH "/Vpdf.config");
	if(LocaleBase && !catalog) {
	    /* try to load the catalog from moduledir/Catalogs first */
	    ULONG d=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_ModulesDir);
	    if(d) {
		BPTR lock=Lock((char*)d,ACCESS_READ);
		BPTR old=SetProgramDir(lock);
		catalog=OpenCatalogA(NULL,"Apdf.catalog",NULL);
		SetProgramDir(old);
		UnLock(lock);
	    }
	    /* if it was not found, maybe the user has moved it
	     * to LOCALE: or voyager/Catalogs.
	     */
	    if(!catalog)
		catalog=OpenCatalogA(NULL,"Apdf.catalog",NULL);
	}
    }
}

void do_save_prefs(struct vplug_prefs *prefs) {
    DB(FPrintf(file,"save_prefs\n");)
    if(config)
	save_prefs(config,VPLUGPREFSPATH "/Vpdf.config");
}

const char *get_prt_dev(void) {
    ULONG d=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_PrintDev);
    return d?(const char*)d:"PRT:";
}


APTR getclass(void) {
    char *modules[16];
    char **pmodule;

    DB(FPrintf(file,"getclass\n");)

    CyberGfxBase=OpenLibrary("cybergraphics.library",41); /* may fail */
    get_config();
    if(config!=NULL) {
	ULONG d;
	BOOL ppcmodule=FALSE;
	BPTR olddir;
	int ok=0;

	d=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_ModulesDir);
	if(d)
	    modulesdir=Lock((char*)d,ACCESS_READ);
	else
	    modulesdir=Lock("PROGDIR:Plugins",ACCESS_READ);
	olddir=CurrentDir(modulesdir);
	DB(FPrintf(file,"modulesdir=%lx\n",modulesdir);)

	pmodule=modules;
	d=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_Module);
	if(d)
	    *pmodule++=(char*)d;

#ifdef HAVE_POWERUP
	if(d) {
	    BPTR file=Open((char*)d,MODE_OLDFILE);
	    if(file) {
		char buf[4];
		Read(file,buf,4);
		if(buf[0]=='\177' && buf[1]=='E' && buf[2]=='L' && buf[3]=='F')
		    ppcmodule=TRUE;
		Close(file);
	    }
	}
	if(!d || ppcmodule)
	    PPCLibBase=OpenLibrary("ppc.library",46);
#endif

	if(!d) {
#ifdef HAVE_POWERUP
	    if(PPCLibBase) {
		ULONG cpu;
		PPCGetAttrsTags(PPCINFOTAG_CPU,(ULONG)&cpu,TAG_END);
		if(cpu==CPU_603e) {
		    *pmodule++="Apdf_603e.module";
		    *pmodule++="Apdf_604e.module";
		} else {
		    *pmodule++="Apdf_604e.module";
		    *pmodule++="Apdf_603e.module";
		}
	    }
#endif
#ifndef __MORPHOS__
	    if(SysBase->AttnFlags&AFF_68881) {
		if(SysBase->AttnFlags&AFF_68060) {
		    *pmodule++="Apdf_68060fpu.module";
		    *pmodule++="Apdf_68040fpu.module";
		} else if(SysBase->AttnFlags&AFF_68040) {
		    *pmodule++="Apdf_68040fpu.module";
		    *pmodule++="Apdf_68060fpu.module";
		}
		if(SysBase->AttnFlags&AFF_68030) {
		    *pmodule++="Apdf_68030fpu.module";
		    *pmodule++="Apdf_68020fpu.module";
		} else if(SysBase->AttnFlags&AFF_68020) {
		    *pmodule++="Apdf_68020fpu.module";
		    *pmodule++="Apdf_68030fpu.module";
		}
	    }
	    if(SysBase->AttnFlags&AFF_68060) {
		*pmodule++="Apdf_68060.module";
		*pmodule++="Apdf_68040.module";
	    } else if(SysBase->AttnFlags&AFF_68040) {
		*pmodule++="Apdf_68040.module";
		*pmodule++="Apdf_68060.module";
	    }
	    if(SysBase->AttnFlags&AFF_68030) {
		*pmodule++="Apdf_68030.module";
		*pmodule++="Apdf_68020.module";
	    } else if(SysBase->AttnFlags&AFF_68020) {
		*pmodule++="Apdf_68020.module";
		*pmodule++="Apdf_68030.module";
	    }
#endif
	}
	*pmodule=NULL;

	for(pmodule=modules;*pmodule!=NULL;++pmodule) {
	    if(
#ifdef HAVE_POWERUP
	       (PPCLibBase && (elfobject=PPCLoadObject(*pmodule))) ||
#endif
	       (seglist=LoadSeg(*pmodule)))
		break;
	}

	if(*pmodule!=NULL) {
	    struct Process *proc=(struct Process*)FindTask(NULL);
	    BPTR old_out,old_err,old_in;
	    DB(FPrintf(file,"module=%s\n",*pmodule);)

#ifdef HAVE_POWERUP
	    if (PPCLibBase && !elfobject) {
		CloseLibrary(PPCLibBase);
		PPCLibBase=NULL;
	    }
#endif

	    if(!proc->pr_CLI) {
		// Setup valid stdin/stdout/stderr for the module,
		// because libnix get confused if it can't open them.
		console_out=Open("CON:////Vpdf/AUTO/WAIT/CLOSE",MODE_NEWFILE);
		console_in=Open("NIL:",MODE_OLDFILE);
		if(console_out && console_in) {
		    old_out=SelectOutput(console_out);
		    old_in=SelectInput(console_in);
		    old_err=proc->pr_CES;
		    proc->pr_CES=console_out;
		}
	    }
	    if(proc->pr_CLI || (console_in && console_out)) {
#ifdef HAVE_POWERUP
		if(elfobject)
		    ok=create_ppc_server(&sv,elfobject);
		else
#endif
		    if(seglist)
			ok=create_m68k_server(&sv,seglist);
		if(!proc->pr_CLI) {
		    proc->pr_CES=old_err;
		    SelectInput(old_in);
		    SelectOutput(old_out);
		}
	    }
	}
	CurrentDir(olddir);

	if(ok) {
	    DB(FPrintf(file,"creating server\n");)
	    if(ci=(*sv.create_comm_info)(&sv,sizeof(union msg_max))) {
		DB(FPrintf(file,"initialising server\n");)
		if((io_server=initIo()) && initsv(ci,VMAGIC,io_server)) {
		    startServer(io_server);
		    DB(FPrintf(file,"initialising classes\n");)
		    if(createDocBitmapClass() && createVDocGroupClass() &&
		       createSimpleDocClass() && createDocLensClass())
			mcc=MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(EmbedData),(APTR)&eDispatcher);
		    if(mcc) {
			module=*pmodule;
			DB(FPrintf(file,"use_prefs\n");)
			use_prefs(ci,config);
			DB(FPrintf(file,"server_ok\n");)
			return mcc->mcc_Class;
		    }
		    deleteSimpleDocClass();
		    deleteVDocGroupClass();
		    deleteDocBitmapClass();
		    deleteDocLensClass();
		}
		cleanupIo(io_server);
            }
	    DB(FPrintf(file,"server creation failed\n");)
	    (*sv.destroy)(&sv);
	}
	if(console_in) {
	    Close(console_in);
	    console_in=0;
	}
	if(console_out) {
	    Close(console_out);
	    console_out=0;
	}
    }
    if(seglist) {
	UnLoadSeg(seglist);
	seglist=0;
    }
#ifdef POWERUP
    if(elfobject) {
	PPCUnLoadObject(elfobject);
	elfobject=NULL;
    }
    CloseLibrary(PPCLibBase);
    PPCLibBase=NULL;
#endif
    UnLock(modulesdir);
    modulesdir=0;
    CloseLibrary(CyberGfxBase);
    CyberGfxBase=NULL;
    return NULL;
}

