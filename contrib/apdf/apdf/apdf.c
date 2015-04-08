/*************************************************************************\
 *                                                                       *
 * Apdf.c                                                                *
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
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>
#include <dos/dostags.h>
#include <exec/nodes.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iffparse.h>
#include <proto/keymap.h>
#include <proto/locale.h>
#ifdef HAVE_POWERUP
#   include <powerup/ppclib/ppc.h>
#   include <powerup/ppclib/tasks.h>
#   include <powerup/ppclib/message.h>
struct ModuleArgs;    /* avoid warnings */
struct PPCObjectInfo; /* avoid warnings */
#   undef NO_INLINE_STDARG
#   ifdef MORPHOS
#       include <proto/ppc.h>
#   else
#       include <powerup/proto/ppc.h>
#   endif
#endif
#include "AComm.h"
#include "Apdf.h"
#include "document.h"
#include "docbitmap.h"
#include "docgroup.h"
#include "docwindow.h"
#include "docscroll.h"
#include "pageset.h"
#include "prefs.h"
#include "msgs.h"
#include "io_client.h"
//#include "notifier.h"

#ifndef AFF_68060
#   define AFF_68060 (1L<<7)
#endif

#define DB(x)   //x
#define Static  static

#define kprintf dprintf

#if HAS_AA
#   define AA(x,y,z) x,y,z
#else
#   define AA(x,y,z)
#endif

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

int create_ppc_server(struct server_info*,void*);
int create_m68k_server(struct server_info*,BPTR);
int create_mos_server(struct server_info*,BPTR);

#if 0

struct MyAllocNode {
    void* ptr;
    size_t size;
};

#define BEGIN_WALL_SIZE 8
#define END_WALL_SIZE   16

void* MyAllocMem(size_t sz) {
    char *p=malloc(sz+sizeof(struct MyAllocNode)+BEGIN_WALL_SIZE+END_WALL_SIZE);
    if (p) {
	struct MyAllocNode *q=(void*)p;
	p+=sizeof(struct MyAllocNode);
	memset(p,0xaa,BEGIN_WALL_SIZE);
	p+=BEGIN_WALL_SIZE;
	memset(p+sz,0xee,END_WALL_SIZE);
	q->ptr=p;
	q->size=sz;
    }
    kprintf("malloc(%ld) = %lx\n",sz,p);
    return p;
}

void MyFreeMem(void* p) {
    kprintf("free(%lx)\n",p);
    if(p) {
	unsigned char *t=(char*)p-BEGIN_WALL_SIZE;
	struct MyAllocNode *q=(void*)(t-sizeof(struct MyAllocNode));
	int k;
	if(q->ptr!=p) {
	    printf("Freeing invalid block: %lx\n",p);
	    exit(20);
	}
	q->ptr=NULL;
	for(k=0;k<BEGIN_WALL_SIZE;++k)
	    if(*t++!=0xaa)
		kprintf("Wall before block %lx trashed. Size = %ld.\n",p,q->size);
	t+=q->size;
	for(k=0;k<END_WALL_SIZE;++k)
	    if(*t++!=0xee)
		kprintf("Wall after block %lx trashed. Size = %ld.\n",p,q->size);
	free(q);
    }
}

#else

void* MyAllocMem(size_t sz) {
    /* needs a malloc that frees all remaining memory at exit */
    return malloc(sz);
}

void MyFreeMem(void* p) {
    free(p);
}

#endif


void CopyStr(char** p,const char* q) {
    if(q) {
	size_t l=strlen(q)+1;
	char* t=MyAllocMem(l);
	if(t) {
	    memcpy(t,q,l);
	    MyFreeMem(*p);
	    *p=t;
	}
    }
}

Object *config;

/*void lock(void) {}

void unlock(void) {}*/

static char* module_arg;
static char* config_arg;
static char* page_arg;
static char* pubscr_arg;
static int zoom_arg=-1;
#define TEMPLATE "PDFFILE,P=PAGE/K,Z=ZOOM/N/K,C=CONFIG/K," \
		 "MODULE/K,NOPOWERUP/S,PUBSCREEN/K"
#define PDFFILE ((const char*)args[0])
#define PAGE    (args[1]?(char*)args[1]:page_arg)
#define ZOOM    (args[2]?*(int*)args[2]:zoom_arg)
#define CONFIG  (args[3]?(const char*)args[3]:(config_arg?config_arg:"PROGDIR:Apdf.config"))
#define MODULE  (args[4]?(char*)args[4]:module_arg)
#define NOPUP   (args[5])
#define PUBSCR  (args[6]?(const char*)args[6]:pubscr_arg)
#define NARGS   7
#define SET_ZOOM(x)    (args[2]=0,zoom_arg=x)
#define SET_NOPUP(x)   (args[5]=x)
static LONG args[NARGS];


#if defined(__SASC) || defined(__libnix__) || defined(MORPHOS)
char __stdiowin[]="CON://600/200/Apdf/AUTO/WAIT/CLOSE";
extern struct WBStartup *_WBenchMsg;

Static void parse_tooltypes(BPTR lock,const char* name) {
    struct DiskObject *dob;
    BPTR olddir=CurrentDir(lock);
    if(dob=GetDiskObject((char*)name)) {
	if(dob->do_ToolTypes) {
	    char *s;
	    /*s=(char*)FindToolType((UBYTE**)dob->do_ToolTypes,(UBYTE*)"PAGE");
	    if(s)
		CopyStr(&page_arg,s);
	    s=(char*)FindToolType((UBYTE**)dob->do_ToolTypes,(UBYTE*)"ZOOM");
	    if(s)
		SET_ZOOM(atoi(s));*/
	    s=(char*)FindToolType((UBYTE**)dob->do_ToolTypes,(UBYTE*)"CONFIG");
	    if(s)
		CopyStr(&config_arg,s);
	    s=(char*)FindToolType((UBYTE**)dob->do_ToolTypes,(UBYTE*)"MODULE");
	    if(s)
		CopyStr(&module_arg,s);
	    s=(char*)FindToolType((UBYTE**)dob->do_ToolTypes,(UBYTE*)"NOPOWERUP");
	    if(s)
		SET_NOPUP(TRUE);
	    s=(char*)FindToolType((UBYTE**)dob->do_ToolTypes,(UBYTE*)"PUBSCREEN");
	    if(s)
		CopyStr(&pubscr_arg,s);
	}
	FreeDiskObject(dob);
    }
    CurrentDir(olddir);
}
#endif


#define ID_ABOUT          1
#define ID_ABOUTMUI       2
#define ID_SEARCH         3
#define ID_OPEN           4
#define ID_SAVEAS         5
#define ID_WRITE          6
#define ID_MUIPREFS       7
#define ID_COPY           8
#define ID_DEFAULTFONTS   9
#define ID_DOCFONTS       11
#define ID_APPLYFONTMAP   12
#define ID_SCANFONTS      13
#define ID_OPENSCANFONTS  15
#define ID_PRINT          16
#define ID_HIDE           17
#define ID_NEW            18
#define ID_DELETE         19
#define ID_NEWVIEW        20
#define ID_LEFT           21
#define ID_RIGHT          22
#define ID_REFRESH        23
#define ID_CROP           24
#define ID_UNCROP         25
#define ID_OUTLINES       26
#define ID_INFO           27
#define ID_TEXTAA         28
#define ID_STROKEAA       29
#define ID_FILLAA         30
#define ID_PREFS          31
#define ID_SAVEPREFS      32
#define ID_USEPREFS       33
#define ID_CANCELPREFS    34
#define ID_LOG            35
#define ID_FONTS          36
#define ID_ADDROW         37
#define ID_REMROW         38
#define ID_ADDCOLUMN      39
#define ID_REMCOLUMN      40
#define ID_CONTINUOUS     41
#define ID_ONEPASS        42
#define ID_TEXTFIRST      43
#define ID_PROGRESSIVE    44
#define ID_NAVBAR         45
#define ID_TOOLBAR        46
#define ID_IMPORTFDF      47
#define ID_EXPORTFDF      48
#define ID_FULLSCREEN     49

#define MYASSERT(x) if(!(x)) { printf("Internal error: %s/%d\n",__FILE__,__LINE__); exit(EXIT_FAILURE); }


#if defined(__SASC) || defined(__libnix__)
long __stack=30000;
#endif

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *UtilityBase;
struct Library *MUIMasterBase;
struct Library *LayersBase;
struct Library *IconBase;
struct Library *IFFParseBase;
struct Library *KeymapBase;
struct Library *LocaleBase;
struct Library *CyberGfxBase;
#ifdef HAVE_POWERUP
struct Library *PPCLibBase;
#endif

struct Catalog *catalog;
/*BPTR input,oldinput;*/
BPTR progdir,modules_dir;
char *progname;
struct RDArgs *rda;
struct DiskObject *diskobj;
struct MUI_CustomClass *docwindow_mcc;
Object *App;
Object *win;
Object *prefswin,*prefsobj;
Object *logwin,*logobj;
struct FileRequester *req;
static BPTR olddir;
APTR oldwindowptr;
static struct comm_info *ci;
struct server_info sv;
static Object *toDelete;
static int win_count;
static BPTR seglist;
#ifdef HAVE_POWERUP
static void *elfobject;
#endif
static int sv_init;
static IoServer io_server;
struct Process* proc;
static ULONG err_quiet;
BOOL is38;

struct DocWindowData {
    struct DocWindowData *next;
    Object *obj;
    Object *doc;
    Object *groupobj;
    Object *menu;
    char *title;
    //Object *notifier;
    int full_screen;
};
typedef struct DocWindowData DocWindowData;

static DocWindowData *windows;

/*
 *  Misc functions.
 */

const char *get_prt_dev(void) {
    ULONG d=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_PrintDev);
    return d?(const char*)d:"PRT:";
}

const char *get_def_icon(void) {
    ULONG d=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_DefIcon);
    return d?(const char*)d:"ENV:sys/def_pdf";
}

BEGIN_STATIC_HOOK(void,intuimsg,
		struct Hook *,h,
		struct FileRequester *,req,
		struct IntuiMessage *,imsg) {
	if (req && App && imsg->Class==IDCMP_REFRESHWINDOW)
		DoMethod(App,MUIM_Application_CheckRefresh);
}
END_STATIC_HOOK(intuimsg)

Static int open_req(Object *win) {
    struct Screen *scr=NULL;
    APTR w=NULL;
    if(win) {
	get(win,MUIA_Window_Screen,&scr);
	get(win,MUIA_Window_Window,&w);
	if(w)
	    proc->pr_WindowPtr=w;
    }
    if((req ||
	(req=MUI_AllocAslRequestTags(ASL_FileRequest,
				     TAG_END))) &&
	MUI_AslRequestTags(req,
			   scr?ASLFR_Screen:TAG_IGNORE,scr,
			   ASLFR_TitleText,STR(MSG_OPEN_REQ_TITLE),
			   ASLFR_PositiveText,STR(MSG_OPEN_REQ_POS),
			   ASLFR_NegativeText,STR(MSG_OPEN_REQ_NEG),
			   ASLFR_RejectIcons,TRUE,
			   ASLFR_DoPatterns,TRUE,
			   ASLFR_DoSaveMode,FALSE,
			   ASLFR_InitialPattern,"#?.(pdf|fdf)",
			   ASLFR_IntuiMsgFunc,&intuimsg_hook,
			   ASLFR_Window,w,
			   TAG_END)) {
	proc->pr_WindowPtr=oldwindowptr;
	return 1;
    }
    proc->pr_WindowPtr=oldwindowptr;
    return 0;
}

Static int save_req(Object *win) {
    struct Screen *scr=NULL;
    if(win) {/* NULL for the initial requester */
	APTR w;
	get(win,MUIA_Window_Screen,&scr);
	get(win,MUIA_Window_Window,&w);
	if(w)
	    proc->pr_WindowPtr=w;
    }
    if((req ||
	(req=MUI_AllocAslRequestTags(ASL_FileRequest,
				     TAG_END))) &&
	MUI_AslRequestTags(req,
			   scr?ASLFR_Screen:TAG_IGNORE,scr,
			   ASLFR_TitleText,STR(MSG_SAVEAS_REQ),
			   ASLFR_PositiveText,STR(MSG_SAVEAS_REQ_POS),
			   ASLFR_NegativeText,STR(MSG_SAVEAS_REQ_NEG),
			   ASLFR_RejectIcons,TRUE,
			   ASLFR_DoPatterns,TRUE,
			   ASLFR_DoSaveMode,TRUE,
			   ASLFR_InitialPattern,"#?.fdf",
			   ASLFR_IntuiMsgFunc,&intuimsg_hook,
			   TAG_END)) {
	proc->pr_WindowPtr=oldwindowptr;
	return 1;
    }
    proc->pr_WindowPtr=oldwindowptr;
    return 0;
}

void delay_close(Object *w) {
    toDelete=w;
    DoMethod3(App,MUIM_Application_ReturnID,ID_DELETE);
}

/*
 *  Server communication functions.
 */

Static int init(struct comm_info *ci,int version,IoServer io_server) {
    struct msg_init *p=ci->cmddata;
    p->version=version;
    p->errors=1;
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

Static int get_error(struct comm_info *ci,char *buf,size_t sz) {
    struct msg_error *p=ci->cmddata;
    if((*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_GETERROR)) {
	strncpy(buf,p->msg,sz-1);
	buf[sz-1]='\0';
	return 1;
    } else
	return 0;
}

Static int import_fdf(struct comm_info *ci, struct PDFDoc *doc, const char *name) {
    struct msg_fdf *p=ci->cmddata;
    sync();
    p->pdfdoc=doc;
    strncpy(p->filename,name,sizeof(p->filename));
    p->filename[sizeof(p->filename)-1]='\0';
    return (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_IMPORTFDF);
}

Static int export_fdf(struct comm_info *ci, struct PDFDoc *doc, const char *name) {
    struct msg_fdf *p=ci->cmddata;
    sync();
    p->pdfdoc=doc;
    strncpy(p->filename,name,sizeof(p->filename));
    p->filename[sizeof(p->filename)-1]='\0';
    return (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_EXPORTFDF);
}

/*Static int scan_default_fonts(void) {
    struct msg_scan_fonts* p=cmddata;
    PPCSendMessage(ppcport,cmdmsg,p,sizeof(*p),MSGID_DEFAULTFONTS);
    PPCWaitPort(reply_port);
    PPCGetMessage(reply_port);
    return p->base.success;
    return 0;
}*/


/*
 * Document window class
 */

#define DocWindowObject NewObject(docwindow_mcc->mcc_Class,NULL

Static struct NewMenu menudata[]={
    {NM_TITLE,(char*)MSG_PROJECT_MENU         ,0,0,0,NULL},
    {NM_ITEM ,(char*) MSG_ABOUT_MENU          ,0,0,0,(APTR)ID_ABOUT},
    {NM_ITEM ,(char*) MSG_ABOUT_MUI_MENU      ,0,0,0,(APTR)ID_ABOUTMUI},
    {NM_ITEM ,        NM_BARLABEL             ,0,0,0,NULL},
    {NM_ITEM ,(char*) MSG_NEW_DOC_MENU        ,0,0,0,(APTR)ID_NEW},
    {NM_ITEM ,(char*) MSG_NEW_VIEW_MENU       ,0,0,0,(APTR)ID_NEWVIEW},
    {NM_ITEM ,(char*) MSG_OPEN_MENU           ,0,0,0,(APTR)ID_OPEN},
    {NM_ITEM ,(char*) MSG_SAVEAS_MENU         ,0,0,0,(APTR)ID_SAVEAS},
    {NM_ITEM ,(char*) MSG_PRINT_MENU          ,0,0,0,(APTR)ID_PRINT},
    {NM_ITEM ,        NM_BARLABEL             ,0,0,0,NULL},
    {NM_ITEM ,(char*) MSG_SAVE_FORM_AS_MENU   ,0,0,0,(APTR)ID_EXPORTFDF},
    {NM_ITEM ,(char*) MSG_IMPORT_FDF_MENU     ,0,0,0,(APTR)ID_IMPORTFDF},
    {NM_ITEM ,        NM_BARLABEL             ,0,0,0,NULL},
    {NM_ITEM ,(char*) MSG_HIDE_MENU           ,0,0,0,(APTR)ID_HIDE},
    {NM_ITEM ,(char*) MSG_QUIT_MENU           ,0,0,0,(APTR)MUIV_Application_ReturnID_Quit},
    {NM_TITLE,(char*)MSG_EDIT_MENU            ,0,0,0,NULL},
    {NM_ITEM ,(char*) MSG_COPY_MENU           ,0,0,0,(APTR)ID_COPY},
    {NM_TITLE,(char*)MSG_VIEW_MENU            ,0,0,0,NULL},
    {NM_ITEM ,(char*) MSG_REFRESH_MENU        ,0,0,0,(APTR)ID_REFRESH},
    {NM_ITEM ,(char*) MSG_CROP_MENU           ,0,0,0,(APTR)ID_CROP},
    {NM_ITEM ,(char*) MSG_UNCROP_MENU         ,0,0,0,(APTR)ID_UNCROP},
    {NM_ITEM ,(char*) MSG_ROTATE_LEFT_MENU    ,0,0,0,(APTR)ID_LEFT},
    {NM_ITEM ,(char*) MSG_ROTATE_RIGHT_MENU   ,0,0,0,(APTR)ID_RIGHT},
    {NM_ITEM ,        NM_BARLABEL             ,0,0,0,NULL},
    {NM_ITEM ,(char*) MSG_ADD_ROW_MENU        ,0,0,0,(APTR)ID_ADDROW},
    {NM_ITEM ,(char*) MSG_REM_ROW_MENU        ,0,0,0,(APTR)ID_REMROW},
    {NM_ITEM ,(char*) MSG_ADD_COLUMN_MENU     ,0,0,0,(APTR)ID_ADDCOLUMN},
    {NM_ITEM ,(char*) MSG_REM_COLUMN_MENU     ,0,0,0,(APTR)ID_REMCOLUMN},
    {NM_ITEM ,        NM_BARLABEL             ,0,0,0,NULL},
    {NM_ITEM ,(char*) MSG_INFO_MENU           ,0,0,0,(APTR)ID_INFO},
    {NM_ITEM ,(char*) MSG_OUTLINES_MENU       ,0,0,0,(APTR)ID_OUTLINES},
    {NM_ITEM ,(char*) MSG_FONTS_MENU          ,0,0,0,(APTR)ID_FONTS},
    {NM_ITEM ,(char*) MSG_ERRORS_LOG_MENU     ,0,0,0,(APTR)ID_LOG},
    {NM_TITLE,(char*)MSG_SETTINGS_MENU        ,0,0,0,NULL},
    {NM_ITEM ,(char*) MSG_GLOBAL_SETTINGS_MENU,0,0,0,(APTR)ID_PREFS},
    {NM_ITEM ,(char*) MSG_DOC_SETTINGS_MENU   ,0,0,0,(APTR)ID_DOCFONTS},
    {NM_ITEM ,(char*) MSG_MUI_MENU            ,0,0,0,(APTR)ID_MUIPREFS},
    {NM_ITEM ,        NM_BARLABEL             ,0,0,0,NULL},
#if HAS_AA
    {NM_ITEM ,(char*) MSG_TEXT_AA_MENU        ,0,CHECKIT|MENUTOGGLE|CHECKED,0,(APTR)ID_TEXTAA},
    {NM_ITEM ,(char*) MSG_STROKE_AA_MENU      ,0,CHECKIT|MENUTOGGLE,0,(APTR)ID_STROKEAA},
    {NM_ITEM ,(char*) MSG_FILL_AA_MENU        ,0,CHECKIT|MENUTOGGLE,0,(APTR)ID_FILLAA},
    {NM_ITEM ,        NM_BARLABEL             ,0,0,0,NULL},
#endif
    {NM_ITEM ,(char*) MSG_CONTINUOUS_MENU     ,0,CHECKIT|MENUTOGGLE,0,(APTR)ID_CONTINUOUS},
    {NM_ITEM ,(char*) MSG_DISPLAY_MODE_MENU   ,0,0,0,NULL},
    {NM_SUB  ,(char*)  MSG_ONE_PASS_MENU      ,0,CHECKIT,~1,(APTR)ID_ONEPASS},
    {NM_SUB  ,(char*)  MSG_TEXT_FIRST_MENU    ,0,CHECKIT,~2,(APTR)ID_TEXTFIRST},
    {NM_SUB  ,(char*)  MSG_PROGRESSIVE_MENU   ,0,CHECKIT|CHECKED,~4,(APTR)ID_PROGRESSIVE},
    {NM_ITEM ,        NM_BARLABEL             ,0,0,0,NULL},
    {NM_ITEM ,(char*) MSG_SHOW_NAV_BAR_MENU   ,0,CHECKIT|CHECKED|MENUTOGGLE,0,(APTR)ID_NAVBAR},
    {NM_ITEM ,(char*) MSG_SHOW_TOOL_BAR_MENU  ,0,CHECKIT|CHECKED|MENUTOGGLE,0,(APTR)ID_TOOLBAR},
    {NM_ITEM ,(char*) MSG_FULL_SCREEN_MENU    ,0,CHECKIT|MENUTOGGLE,0,(APTR)ID_FULLSCREEN},
    {NM_END  ,NULL                            ,0,0,0,NULL}
};

BEGIN_STATIC_HOOK(ULONG,appwin,
		  struct Hook *,h,
		  Object *,obj,
		  struct AppMessage**,p) {
    struct AppMessage *msg=*p;
    Object *app;
    get(obj,MUIA_ApplicationObject,&app);
    if(msg->am_Version>=AM_VERSION && msg->am_NumArgs>=1)
	set(obj,MYATTR_DocWindow_Document,getDocument(ci,app,obj,msg->am_ArgList->wa_Lock,msg->am_ArgList->wa_Name));
    return 0;
}
END_STATIC_HOOK(appwin)

Static ULONG dwNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    DocWindowData* dat;
    struct TagItem tags[15];
    Object *menu,*groupobj;
    Object *doc=(Object*)GetTagData(MYATTR_DocWindow_Document,0,msg->ops_AttrList);
    int num_pages,page,n;
    BOOL full_screen=GetTagData(MYATTR_DocWindow_FullScreen,0,msg->ops_AttrList);
    const char *pagelabel;
    char *title = NULL;

    if(!doc)
	return 0;

    page=GetTagData(MYATTR_DocWindow_Page,-1,msg->ops_AttrList);
    pagelabel=(const char*)GetTagData(MYATTR_DocWindow_PageLabel,0,msg->ops_AttrList);

    tags[0].ti_Tag=MUIA_Window_Menustrip;
    tags[0].ti_Data=(ULONG)(menu=MUI_MakeObject(MUIO_MenustripNM,menudata,0));
    tags[1].ti_Tag=MUIA_Window_AppWindow;
    tags[1].ti_Data=TRUE;
    tags[2].ti_Tag=MUIA_Window_RootObject;
    tags[2].ti_Data=(ULONG)(groupobj=DocGroupObject,
	MYATTR_DocGroup_CommInfo,ci,
	MYATTR_DocGroup_Document,doc,
	MYATTR_DocGroup_FullScreen,full_screen,
	page==-1?TAG_IGNORE:MYATTR_DocGroup_Page,page,
	pagelabel==NULL?TAG_IGNORE:MYATTR_DocGroup_PageLabel,pagelabel,
	End);
    tags[3].ti_Tag=MUIA_HelpNode;
    tags[3].ti_Data=(ULONG)"mainwin";
    tags[4].ti_Tag=MUIA_Window_PublicScreen;
    tags[4].ti_Data=(ULONG)PUBSCR;
    if(full_screen) {
	SetAttrs(groupobj,
		 MYATTR_DocGroup_ShowNavBar,FALSE,
		 MYATTR_DocGroup_ShowToolBar,FALSE,
		 TAG_END);
	tags[5].ti_Tag=MUIA_Window_CloseGadget;
	tags[5].ti_Data=FALSE;
	tags[6].ti_Tag=MUIA_Window_DragBar;
	tags[6].ti_Data=FALSE;
	tags[7].ti_Tag=MUIA_Window_DepthGadget;
	tags[7].ti_Data=FALSE;
	tags[8].ti_Tag=MUIA_Window_SizeGadget;
	tags[8].ti_Data=FALSE;
	tags[9].ti_Tag=MUIA_Window_Borderless;
	tags[9].ti_Data=TRUE;
	tags[10].ti_Tag=MUIA_Window_Width;
	tags[10].ti_Data=MUIV_Window_Width_Screen(100);
	tags[11].ti_Tag=MUIA_Window_Height;
	tags[11].ti_Data=MUIV_Window_Height_Screen(100);
	n=12;
    } else {
	char *doctitle;
	get(doc,MYATTR_Document_Name,&doctitle);
	CopyStr(&title, doctitle);
	tags[5].ti_Tag=MUIA_Window_Title;
	tags[5].ti_Data = title;
	tags[6].ti_Tag=MUIA_Window_ID;
	tags[6].ti_Data=MAKE_ID('A','P','D','F');
	n=7;
    }
    tags[n].ti_Tag=TAG_MORE;
    tags[n].ti_Data=(ULONG)msg->ops_AttrList;
    msg->ops_AttrList=tags;

    if(obj=(Object *)DoSuperMethodA(cl,obj,(Msg)msg)) {
	int n;
	Object *tmp;
	dat=INST_DATA(cl,obj);
	dat->next=windows;
	windows=dat;
	dat->obj=obj;
	dat->doc=doc;
	dat->title=title;
	dat->groupobj=groupobj;
	dat->menu=menu;
	dat->full_screen=full_screen;
	get(groupobj,MYATTR_DocGroup_MainObject,&tmp);
	set(obj,MUIA_Window_DefaultObject,tmp);

	DoMethod7(obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
		 obj,1,MYM_DocWindow_Close);
	DoMethod9(obj,MUIM_Notify,MUIA_AppMessage,MUIV_EveryTime,
		 obj,3,MUIM_CallHook,&appwin_hook,MUIV_TriggerValue);
	DoMethod9(groupobj,MUIM_Notify,MYATTR_DocGroup_DocChanged,MUIV_EveryTime,
		 obj,3,MUIM_Set,MYATTR_DocWindow_Document,MUIV_TriggerValue);
	DoMethod8(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_ABOUTMUI,
		 App,2,MUIM_Application_AboutMUI,obj);
	DoMethod8(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_MUIPREFS,
		 App,2,MUIM_Application_OpenConfigWindow,0);
	DoMethod9(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_PREFS,
		 prefswin,3,MUIM_Set,MUIA_Window_Open,TRUE);
	DoMethod9(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_HIDE,
		 App,3,MUIM_Set,MUIA_Application_Iconified,TRUE);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_COPY,
		 groupobj,1,MYM_PageSet_ToClip);
	DoMethod8(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_OPEN,
		 obj,2,MYM_DocWindow_Open,NULL);
	DoMethod8(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_NEW,
		 obj,2,MYM_DocWindow_OpenNew,NULL);
	DoMethod8(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_NEWVIEW,
		 obj,2,MYM_DocWindow_NewView,0);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_IMPORTFDF,
		 obj,1,MYM_DocWindow_ImportFDF);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_EXPORTFDF,
		 obj,1,MYM_DocWindow_ExportFDF);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_LEFT,
		 groupobj,1,MYM_PageSet_RLeft);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_RIGHT,
		 groupobj,1,MYM_PageSet_RRight);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_REFRESH,
		 groupobj,1,MYM_PageSet_Refresh);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_CROP,
		 groupobj,1,MYM_PageSet_Crop);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_UNCROP,
		 groupobj,1,MYM_PageSet_UnCrop);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_ADDROW,
		 groupobj,1,MYM_PageSet_AddRow);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_REMROW,
		 groupobj,1,MYM_PageSet_RemRow);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_ADDCOLUMN,
		 groupobj,1,MYM_PageSet_AddColumn);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_REMCOLUMN,
		 groupobj,1,MYM_PageSet_RemColumn);
	DoMethod9(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_ONEPASS,
		 groupobj,3,MUIM_Set,MYATTR_PageSet_DrawMode,DrAllAtOnce);
	DoMethod9(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_TEXTFIRST,
		 groupobj,3,MUIM_Set,MYATTR_PageSet_DrawMode,DrTextFirst);
	DoMethod9(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_PROGRESSIVE,
		 groupobj,3,MUIM_Set,MYATTR_PageSet_DrawMode,DrPartialUpdates);
	DoMethod9(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_LOG,
		 logwin,3,MUIM_Set,MUIA_Window_Open,TRUE);
	get(doc,MYATTR_Document_HasOutlines,&n);
	DoMethod5(dat->menu,MUIM_SetUData,ID_OUTLINES,MUIA_Menuitem_Enabled,n!=0);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_OUTLINES,
		 groupobj,1,MYM_DocGroup_OpenOutlines);
	tmp=(Object*)DoMethod3(menu,MUIM_FindUData,ID_CONTINUOUS);
	DoMethod9(tmp,MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,
		 groupobj,3,MUIM_Set,MYATTR_DocScroll_ViewMode,MUIV_TriggerValue);
	tmp=(Object*)DoMethod3(menu,MUIM_FindUData,ID_NAVBAR);
	DoMethod9(tmp,MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,
		 groupobj,3,MUIM_Set,MYATTR_DocGroup_ShowNavBar,MUIV_TriggerValue);
	tmp=(Object*)DoMethod3(menu,MUIM_FindUData,ID_TOOLBAR);
	DoMethod9(tmp,MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,
		 groupobj,3,MUIM_Set,MYATTR_DocGroup_ShowToolBar,MUIV_TriggerValue);
#if HAS_AA
	tmp=(Object*)DoMethod3(menu,MUIM_FindUData,ID_TEXTAA);
	DoMethod9(tmp,MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,
		 groupobj,3,MUIM_Set,MYATTR_DocGroup_TextAA,MUIV_TriggerValue);
	get(doc,MYATTR_Document_OpenTextAA,&n);
	set(tmp,MUIA_Menuitem_Checked,GetTagData(MYATTR_DocWindow_TextAA,n,msg->ops_AttrList));
	tmp=(Object*)DoMethod3(menu,MUIM_FindUData,ID_STROKEAA);
	DoMethod9(tmp,MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,
		 groupobj,3,MUIM_Set,MYATTR_DocGroup_StrokeAA,MUIV_TriggerValue);
	get(doc,MYATTR_Document_OpenStrokeAA,&n);
	set(tmp,MUIA_Menuitem_Checked,GetTagData(MYATTR_DocWindow_StrokeAA,n,msg->ops_AttrList));
	tmp=(Object*)DoMethod3(menu,MUIM_FindUData,ID_FILLAA);
	DoMethod9(tmp,MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,
		 groupobj,3,MUIM_Set,MYATTR_DocGroup_FillAA,MUIV_TriggerValue);
	get(doc,MYATTR_Document_OpenFillAA,&n);
	set(tmp,MUIA_Menuitem_Checked,GetTagData(MYATTR_DocWindow_FillAA,n,msg->ops_AttrList));
#endif
	DoMethod8(groupobj,MUIM_Notify,MYATTR_DocGroup_Run,MUIV_EveryTime,
		 obj,2,MYM_DocWindow_Run,MUIV_TriggerValue);
	DoMethod8(groupobj,MUIM_Notify,MYATTR_DocGroup_URL,MUIV_EveryTime,
		 obj,2,MYM_DocWindow_URL,MUIV_TriggerValue);
	DoMethod8(groupobj,MUIM_Notify,MYATTR_DocGroup_Open,MUIV_EveryTime,
		 obj,2,MYM_DocWindow_Open,MUIV_TriggerValue);
	DoMethod8(groupobj,MUIM_Notify,MYATTR_DocGroup_OpenNew,MUIV_EveryTime,
		 obj,2,MYM_DocWindow_OpenNew,MUIV_TriggerValue);
	DoMethod8(groupobj,MUIM_Notify,MYATTR_DocGroup_PageNew,MUIV_EveryTime,
		 obj,2,MYM_DocWindow_NewView,MUIV_TriggerValue);
	/* Don't trigger methods of the Document class directly,
	   because dat->doc can change. */
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_PRINT,
		 obj,1,MYM_DocWindow_OpenPrintWindow);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_SAVEAS,
		 obj,1,MYM_DocWindow_OpenSaveWindow);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_INFO,
		 obj,1,MYM_DocWindow_OpenInfoWindow);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_DOCFONTS,
		 obj,1,MYM_DocWindow_OpenFontMapWindow);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_MenuAction,ID_FONTS,
		 obj,1,MYM_DocWindow_OpenScanWindow);
	DoMethod7(obj,MUIM_Notify,MUIA_Window_Activate,FALSE,
		 groupobj,1,MYM_DocBitmap_DestroyLens);
	tmp=(Object*)DoMethod3(menu,MUIM_FindUData,ID_FULLSCREEN);
	set(tmp,MUIA_Menuitem_Checked,full_screen);
	DoMethod8(tmp,MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,
		  obj,2,MYM_DocWindow_FullScreen,MUIV_TriggerValue);
	++win_count;

	n=GetTagData(MYATTR_DocWindow_Zoom,minZoom-1,msg->ops_AttrList);
	DB(kprintf("DocWindow::new zoom %d\n",n);)
	if(n!=minZoom-1)
	    set(groupobj,MYATTR_DocGroup_Zoom,n);
	n=GetTagData(MYATTR_DocWindow_Rotate,-1,msg->ops_AttrList);
	if(n!=-1)
	    set(groupobj,MYATTR_DocGroup_Rotate,n);
	n=GetTagData(MYATTR_DocWindow_CropBox,0,msg->ops_AttrList);
	if(n!=0)
	    set(groupobj,MYATTR_DocGroup_CropBox,n);
	get(doc,MYATTR_Document_OpenDispMode,&n);
	DoMethod5(menu,MUIM_SetUDataOnce,ID_ONEPASS+n,MUIA_Menuitem_Checked,TRUE);
	if(full_screen)
	    n=FALSE;
	else
	    get(doc,MYATTR_Document_OpenToolBar,&n);
	DoMethod5(menu,MUIM_SetUDataOnce,ID_TOOLBAR,MUIA_Menuitem_Checked,n);
	if(full_screen)
	    n=FALSE;
	else
	    get(doc,MYATTR_Document_OpenNavBar,&n);
	DoMethod5(menu,MUIM_SetUDataOnce,ID_NAVBAR,MUIA_Menuitem_Checked,n);
	get(doc,MYATTR_Document_OpenColumns,&n);
	if(n>=1)
	    set(groupobj,MYATTR_PageSet_Columns,n);
	get(doc,MYATTR_Document_OpenRows,&n);
	if(n>=1)
	    set(groupobj,MYATTR_PageSet_Rows,n);
	if(full_screen)
	    n=FALSE;
	else
	    get(doc,MYATTR_Document_OpenContinuous,&n);
	DoMethod5(menu,MUIM_SetUDataOnce,ID_CONTINUOUS,MUIA_Menuitem_Checked,n);
    }
    msg->ops_AttrList=(struct TagItem*)tags[n].ti_Data;
    return (ULONG)obj;
}



Static ULONG dwDispose(struct IClass *cl,Object *obj,Msg msg) {
    DocWindowData *dat=INST_DATA(cl,obj);
    DocWindowData **p=&windows;
    while(*p!=dat)
	p=&(*p)->next;
    *p=dat->next;
    --win_count;
    //DisposeObject(dat->notifier);
    MyFreeMem(dat->title);
    dat->title = NULL;
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG dwGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    switch(msg->opg_AttrID) {
      case MYATTR_DocWindow_Document: {
	DocWindowData* dat=INST_DATA(cl,obj);
	*msg->opg_Storage=(ULONG)dat->doc;
	return TRUE;
      }
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

Static ULONG dwSet(struct IClass *cl,Object *obj,struct opSet *msg) {
    DocWindowData *dat=INST_DATA(cl,obj);
    struct TagItem *tags=msg->ops_AttrList;
    struct TagItem *tag;
    while(tag=NextTagItem(&tags)) {
	switch(tag->ti_Tag) {
	  case  MYATTR_DocWindow_Document: {
	    Object *doc=(Object*)tag->ti_Data;
	    const char *name;
	    if(doc && doc!=dat->doc) {
		int f;
		dat->doc=doc;
		set(dat->groupobj,MYATTR_DocGroup_Document,doc);
		get(doc,MYATTR_Document_HasOutlines,&f);
		DoMethod5(dat->menu,MUIM_SetUData,ID_OUTLINES,MUIA_Menuitem_Enabled,f!=0);
		/*DisposeObject(dat->notifier);
		dat->notifier=NULL;*/
	    }
	    MyFreeMem(dat->title);
	    dat->title = NULL;
	    get(dat->doc,MYATTR_Document_Name,&name);
	    CopyStr(&dat->title, name);
	    set(obj,MUIA_Window_Title,dat->title);
	    /*if(!dat->notifier) {
		BPTR dir;
		BPTR olddir;
		get(dat->doc,MYATTR_Document_Dir,&dir);
		olddir=CurrentDir(dir);
		dat->notifier=NotifierObject,
		    MYATTR_Notifier_Name,name,
		    End;
		CurrentDir(olddir);
		if(dat->notifier) {
		    DoMethod7(dat->notifier,MUIM_Notify,MYATTR_Notifier_Changed,TRUE,
			      obj,1,MYM_DocWindow_Reload);
		}
	    }*/
	    break ;
	  }
	  case MYATTR_DocWindow_Page:
	    set(dat->groupobj,MYATTR_DocGroup_Page,tag->ti_Data);
	    break;
	  case MYATTR_DocWindow_PageLabel:
	    set(dat->groupobj,MYATTR_DocGroup_PageLabel,tag->ti_Data);
	    break;
	  case MYATTR_DocWindow_Zoom:
	    DB(kprintf("DocWindow::set zoom %d\n",tag->ti_Data);)
	    set(dat->groupobj,MYATTR_DocGroup_Zoom,tag->ti_Data);
	    break;
	  case MYATTR_DocWindow_Rotate:
	    set(dat->groupobj,MYATTR_DocGroup_Rotate,tag->ti_Data);
	    break;
	}
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

struct MYP_DocWindow_Open {
    ULONG MethodID;
    const char *FileName;
};

Static ULONG dwOpen(struct IClass *cl,Object *obj,struct MYP_DocWindow_Open *msg) {
    DocWindowData *dat=INST_DATA(cl,obj);
    BPTR dir,olddir,lock=0;
    Object *app,*doc=NULL;
    const char *name=NULL;
    get(dat->doc,MYATTR_Document_Dir,&dir);
    olddir=CurrentDir(dir);
    get(obj,MUIA_ApplicationObject,&app);
    if(msg->FileName) {
	lock=DupLock(dir);
	name=msg->FileName;
	while(name[0]=='.' && name[1]=='.' && name[2]=='/') {
	    BPTR dir2=ParentDir(dir);
	    UnLock(dir);
	    dir=dir2;
	    name+=3;
	}
    } else if(open_req(obj)) {
	lock=Lock(req->fr_Drawer,ACCESS_READ);
	name=req->fr_File;
    }
    if(name)
	doc=getDocument(ci,app,obj,lock,name);
    UnLock(lock);
    if(doc)
	set(dat->groupobj,MYATTR_DocGroup_Document,doc);
    CurrentDir(olddir);
    return 0;
}

Static ULONG dwOpenNew(struct IClass *cl,Object *obj,struct MYP_DocWindow_Open *msg) {
    DocWindowData *dat=INST_DATA(cl,obj);
    BPTR dir,olddir,lock=0;
    Object *app;
    Object *doc=NULL;
    Object *w;
    const char *name=NULL;
    get(dat->doc,MYATTR_Document_Dir,&dir);
    olddir=CurrentDir(dir);
    get(obj,MUIA_ApplicationObject,&app);
    if(msg->FileName) {
	lock=DupLock(dir);
	name=msg->FileName;
	while(name[0]=='.' && name[1]=='.' && name[2]=='/') {
	    BPTR dir2=ParentDir(dir);
	    UnLock(dir);
	    dir=dir2;
	    name+=3;
	}
    } else if(open_req(obj)) {
	lock=Lock(req->fr_Drawer,ACCESS_READ);
	name=req->fr_File;
    }
    if(name)
	doc=getDocument(ci,app,NULL,lock,name);
    UnLock(lock);
    w=DocWindowObject,
	MYATTR_DocWindow_Document,doc,
	End;
    if(w) {
	DoMethod3(app,OM_ADDMEMBER,w);
	set(w,MUIA_Window_Open,TRUE);
    }
    CurrentDir(olddir);
    return 0;
}

#if 0
Static ULONG dwReload(struct IClass *cl,Object *obj) {
    DocWindowData *dat=INST_DATA(cl,obj);
    BPTR dir;
    Object *app,*doc=NULL;
    char *name=NULL;
    char c;
    get(dat->doc,MYATTR_Document_Dir,&dir);
    get(dat->doc,MYATTR_Document_Name,&name);
    get(obj,MUIA_ApplicationObject,&app);
    c=*name; /* ugly hack */
    *name='\0';
    doc=getDocument(ci,app,obj,dir,name);
    *name=c;
    if(doc)
	set(dat->groupobj,MYATTR_DocGroup_Document,doc);
    return 0;
}
#endif

struct MYP_DocWindow_NewView {
    ULONG MethodID;
    int page;
};

Static ULONG dwNewView(struct IClass *cl,Object *obj,struct MYP_DocWindow_NewView *msg) {
    DocWindowData *dat=INST_DATA(cl,obj);
    Object *w;
    int page,zoom,rotate;
    struct MYS_DocBitmap_CropBox box;
    int textaa,fillaa,strokeaa;
    if(msg->page>=1)
	page=msg->page;
    else
	get(dat->groupobj,MYATTR_DocGroup_Page,&page);
    get(dat->groupobj,MYATTR_DocGroup_Zoom,&zoom);
    get(dat->groupobj,MYATTR_DocGroup_Rotate,&rotate);
    get(dat->groupobj,MYATTR_DocGroup_CropBox,&box);
#if HAS_AA
    get(dat->groupobj,MYATTR_DocGroup_TextAA,&textaa);
    get(dat->groupobj,MYATTR_DocGroup_StrokeAA,&strokeaa);
    get(dat->groupobj,MYATTR_DocGroup_FillAA,&fillaa);
#endif
    w=DocWindowObject,
	MYATTR_DocWindow_Document,dat->doc,
	MYATTR_DocWindow_Page,page,
	MYATTR_DocWindow_Zoom,zoom,
	MYATTR_DocWindow_Rotate,rotate,
	MYATTR_DocWindow_CropBox,&box,
	AA(MYATTR_DocWindow_TextAA,textaa,)
	AA(MYATTR_DocWindow_StrokeAA,strokeaa,)
	AA(MYATTR_DocWindow_FillAA,fillaa,)
	End;
    if(w) {
	DoMethod3(App,OM_ADDMEMBER,w);
	set(w,MUIA_Window_Open,TRUE);
    }
    return 0;
}

struct MYP_DocWindow_FullScreen {
    ULONG MethodID;
    int flag;
};


Static ULONG dwFullScreen(struct IClass *cl,Object *obj, struct MYP_DocWindow_FullScreen *msg) {
    DocWindowData *dat=INST_DATA(cl,obj);
    Object *w;
    int page,zoom,rotate;
    struct MYS_DocBitmap_CropBox box;
    int textaa,fillaa,strokeaa;
    get(dat->groupobj,MYATTR_DocGroup_Page,&page);
    get(dat->groupobj,MYATTR_DocGroup_Rotate,&rotate);
    get(dat->groupobj,MYATTR_DocGroup_CropBox,&box);
#if HAS_AA
    get(dat->groupobj,MYATTR_DocGroup_TextAA,&textaa);
    get(dat->groupobj,MYATTR_DocGroup_StrokeAA,&strokeaa);
    get(dat->groupobj,MYATTR_DocGroup_FillAA,&fillaa);
#endif
    w=DocWindowObject,
	MYATTR_DocWindow_Document,dat->doc,
	MYATTR_DocWindow_Page,page,
	MYATTR_DocWindow_Zoom,zoomPage,
	MYATTR_DocWindow_Rotate,rotate,
	MYATTR_DocWindow_CropBox,&box,
	AA(MYATTR_DocWindow_TextAA,textaa,)
	AA(MYATTR_DocWindow_StrokeAA,strokeaa,)
	AA(MYATTR_DocWindow_FillAA,fillaa,)
	MYATTR_DocWindow_FullScreen,msg->flag,
	End;
    if(w) {
	DoMethod3(App,OM_ADDMEMBER,w);
	set(obj,MUIA_Window_Open,FALSE);
	set(w,MUIA_Window_Open,TRUE);
	delay_close(obj);
    }
    return 0;
}

Static ULONG dwClose(struct IClass *cl,Object *obj) {
    delay_close(obj);
    return 0;
}

Static ULONG dwOpenPrintWindow(struct IClass *cl,Object *obj) {
    DocWindowData *dat=INST_DATA(cl,obj);
    return DoMethod2(dat->doc,MYM_Document_OpenPrintWindow);
}

Static ULONG dwOpenSaveWindow(struct IClass *cl,Object *obj) {
    DocWindowData *dat=INST_DATA(cl,obj);
    return DoMethod2(dat->doc,MYM_Document_OpenSaveWindow);
}

Static ULONG dwOpenInfoWindow(struct IClass *cl,Object *obj) {
    DocWindowData *dat=INST_DATA(cl,obj);
    return DoMethod2(dat->doc,MYM_Document_OpenInfoWindow);
}

Static ULONG dwOpenFontMapWindow(struct IClass *cl,Object *obj) {
    DocWindowData *dat=INST_DATA(cl,obj);
    return DoMethod2(dat->doc,MYM_Document_OpenPrefsWindow);
}

Static ULONG dwOpenScanWindow(struct IClass *cl,Object *obj) {
    DocWindowData *dat=INST_DATA(cl,obj);
    return DoMethod2(dat->doc,MYM_Document_OpenScanWindow);
}

Static ULONG dwURL(struct IClass *cl,Object *obj,struct MYP_DocWindow_Open *msg) {
    ULONG d=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_URLCmd);
    char buf2[256];
    buf2[0]='r';
    buf2[1]='u';
    buf2[2]='n';
    buf2[3]=' ';
    sprintf(buf2+4,d?(const char *)d:"OpenURL %s",msg->FileName);
    SystemTagList(buf2,NULL);
    return 0;
}

Static ULONG dwRun(struct IClass *cl,Object *obj,struct MYP_DocWindow_Open *msg) {
    Object *app;
    get(obj,MUIA_ApplicationObject,&app);
    if(MUI_Request(app,NULL,0,(char*)STR(MSG_EXECUTE_REQ),
		   (char*)STR(MSG_EXECUTE_REQ_ANSWERS),
		   (char*)msg->FileName))
	SystemTagList((char*)msg->FileName,NULL);
    return 0;
}

Static ULONG dwImportFDF(struct IClass *cl,Object *obj) {
    DocWindowData *dat=INST_DATA(cl,obj);
    if(open_req(obj)) {
	char buf[256];
	struct PDFDoc *pdfdoc;
	strncpy(buf,req->fr_Drawer,sizeof(buf));
	buf[sizeof(buf)-1]='\0';
	AddPart(buf,req->fr_File,sizeof(buf));
	get(dat->doc,MYATTR_Document_PDFDoc,&pdfdoc);
	import_fdf(ci,pdfdoc,buf);
	DoMethod2(dat->doc,MYM_Document_RefreshWindows);
    }
    return 0;
}

Static ULONG dwExportFDF(struct IClass *cl,Object *obj) {
    DocWindowData *dat=INST_DATA(cl,obj);
    if(save_req(obj)) {
	char buf[256];
	struct PDFDoc *pdfdoc;
	strncpy(buf,req->fr_Drawer,sizeof(buf));
	buf[sizeof(buf)-1]='\0';
	AddPart(buf,req->fr_File,sizeof(buf));
	get(dat->doc,MYATTR_Document_PDFDoc,&pdfdoc);
	export_fdf(ci,pdfdoc,buf);
	DoMethod2(dat->doc,MYM_Document_RefreshWindows);
    }
    return 0;
}

BEGIN_DISPATCHER(dwDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:               return dwNew    (cl,obj,(APTR)msg);
      case OM_DISPOSE:           return dwDispose(cl,obj,msg);
      case OM_GET:               return dwGet    (cl,obj,(APTR)msg);
      case OM_SET:               return dwSet    (cl,obj,(APTR)msg);
      case MYM_DocWindow_Open:   return dwOpen   (cl,obj,(APTR)msg);
      case MYM_DocWindow_OpenNew:return dwOpenNew(cl,obj,(APTR)msg);
      case MYM_DocWindow_Close:  return dwClose  (cl,obj);
      case MYM_DocWindow_NewView:return dwNewView(cl,obj,(APTR)msg);
      case MYM_DocWindow_OpenPrintWindow: return dwOpenPrintWindow(cl,obj);
      case MYM_DocWindow_OpenSaveWindow:  return dwOpenSaveWindow(cl,obj);
      case MYM_DocWindow_OpenInfoWindow:  return dwOpenInfoWindow(cl,obj);
      case MYM_DocWindow_OpenFontMapWindow:  return dwOpenFontMapWindow(cl,obj);
      case MYM_DocWindow_OpenScanWindow:  return dwOpenScanWindow(cl,obj);
      case MYM_DocWindow_URL:    return dwURL    (cl,obj,(APTR)msg);
      case MYM_DocWindow_Run:    return dwRun    (cl,obj,(APTR)msg);
      case MYM_DocWindow_ImportFDF:return dwImportFDF(cl,obj);
      case MYM_DocWindow_ExportFDF:return dwExportFDF(cl,obj);
      case MYM_DocWindow_FullScreen:return dwFullScreen(cl,obj,(APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(dwDispatcher)


void cleanup(void) {

    proc->pr_WindowPtr=oldwindowptr;

    if(MUIMasterBase) {
	MUI_FreeAslRequest(req);
	/* The following should not be necessary, but without that,
	 * there seems to be crashes when Apdf is quitted
	 * while iconified. No idea why.
	 */
	while(windows) {
	    set(windows->obj,MUIA_Window_Open,FALSE);
	    DoMethod3(App,OM_REMMEMBER,windows->obj);
	    MUI_DisposeObject(windows->obj);
	}
	if(App) {
	    cleanupComm();
	    MUI_DisposeObject(App);
	}
	if(docwindow_mcc)
	    MUI_DeleteCustomClass(docwindow_mcc);
	deleteDocBitmapClass();
	deleteDocGroupClass();
	deleteDocLensClass();
	deleteDocumentClass();
	deletePrefsClass();
	MUI_DisposeObject(config);
	CloseLibrary(MUIMasterBase);
    }

    //cleanupNotifier();

    if(ci)
	(*sv.delete_comm_info)(ci);
    if(sv_init)
	(*sv.destroy)(&sv);
    cleanupIo(io_server);

    if(seglist)
	UnLoadSeg(seglist);
#ifdef HAVE_POWERUP
    if(elfobject)
	PPCUnLoadObject(elfobject);
    CloseLibrary(PPCLibBase);
#endif
    UnLock(modules_dir);

    if(diskobj)
	FreeDiskObject(diskobj);

    if(catalog) /* LocaleBase might be NULL */
	CloseCatalog(catalog);

    CloseLibrary(CyberGfxBase);
    CloseLibrary(KeymapBase);
    CloseLibrary(IFFParseBase);
    CloseLibrary(IconBase);
    CloseLibrary(LayersBase);
    CloseLibrary((struct Library*)LocaleBase);
    CloseLibrary((struct Library*)UtilityBase);
    CloseLibrary((struct Library*)GfxBase);
    CloseLibrary((struct Library*)IntuitionBase);

    FreeArgs(rda);
    CurrentDir(olddir);

    /*if(input) {
	SelectInput(oldinput);
	Close(input);
    }*/
}


BEGIN_STATIC_HOOK(ULONG,rxopen,
		  struct Hook *,h,
		  Object *,app,
		  ULONG *,args) {
    Object *doc=getDocument(ci,app,NULL,progdir,(const char *)args[0]);
    Object *w=DocWindowObject,
	MYATTR_DocWindow_Document,doc,
	MYATTR_DocWindow_PageLabel,args[1],
	args[2]?MYATTR_DocWindow_Zoom:TAG_IGNORE,args[2]?*(LONG*)args[2]+5:0,
	args[3]?MYATTR_DocWindow_Rotate:TAG_IGNORE,args[3]?90*((*(LONG*)args[3]/90)&3):0,
	End;
    if(w) {
	DoMethod3(app,OM_ADDMEMBER,w);
	set(w,MUIA_Window_Open,TRUE);
	return 0;
    } else
	return 10;
}
END_STATIC_HOOK(rxopen)


static struct MUI_Command rexxcmds[]={
    {"OPEN","FILE/A,P=PAGE/K,Z=ZOOM/N/K,R=ROTATE/N/K",4,&rxopen_hook},
    {NULL,NULL,0,NULL}
};

int main() {

    Object *saveprefsobj,*useprefsobj,*cancelprefsobj;
    Object *clearlogobj,*autoopenobj,*win;
    char* docname=NULL;
    BPTR docdir=0;
    BOOL running,ppcmodule;
    char *modules[16];
    char **pmodule;
    ULONG sigs;

    proc=(struct Process*)FindTask(NULL);

    if(LocaleBase=OpenLibrary("locale.library",0))
	catalog=OpenCatalogA(NULL,"Apdf.catalog",NULL);

    /* This test is probably useless. If run on 68000 or */
    /* 68010, we have probably already crashed at this point. */
    if(!(SysBase->AttnFlags&AFF_68020)) {
	printf(STR(MSG_REQUIRES_68020_ERR));
	exit(EXIT_FAILURE);
    }

    olddir=CurrentDir(0);
    CurrentDir(olddir);
    oldwindowptr=proc->pr_WindowPtr;

    atexit(&cleanup);

    IntuitionBase=(struct IntuitionBase*)OpenLibrary("intuition.library",39);
    GfxBase=(struct GfxBase*)OpenLibrary("graphics.library",39);
    UtilityBase=OpenLibrary("utility.library",39);
    LayersBase=OpenLibrary("layers.library",36);
    IconBase=OpenLibrary("icon.library",39);
    IFFParseBase=OpenLibrary("iffparse.library",39);
    KeymapBase=OpenLibrary("keymap.library",0);
    if(!IntuitionBase || !GfxBase || !UtilityBase || !LayersBase ||
       !IconBase || !IFFParseBase || !KeymapBase) {
	printf(STR(MSG_REQUIRES_3_0_ERR));
	exit(EXIT_FAILURE);
    }
    MUIMasterBase=OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN);
    if(!MUIMasterBase) {
	printf(STR(MSG_NO_MUIMASTER_ERR));
	exit(EXIT_FAILURE);
    }
    is38=MUIMasterBase->lib_Version<=19;

    docwindow_mcc=MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(DocWindowData),(APTR)&dwDispatcher);
    if(!createDocBitmapClass() || !createDocGroupClass() ||
       !createDocumentClass() || !createDocLensClass() ||
       !createPrefsClass() || !docwindow_mcc) {
	printf(STR(MSG_CANT_CREATE_CC_ERR));
	exit(EXIT_FAILURE);
    }


    if(!(config=MUI_NewObjectA(MUIC_Dataspace,NULL))) {
	printf(STR(MSG_CANT_CREATE_CONFIG_OBJ_ERR));
	exit(EXIT_FAILURE);
    } else {
	Object *pend=MUI_NewObjectA(MUIC_Pendisplay,NULL);
	if(pend) {
	    struct MUI_PenSpec *spec;
	    DoMethod5(pend,MUIM_Pendisplay_SetRGB,0xffffffff,0,0);
	    get(pend,MUIA_Pendisplay_Spec,&spec);
	    MUI_DisposeObject(pend);
	    if(!DoMethod5(config,MUIM_Dataspace_Add,spec,sizeof(*spec),MYCFG_Apdf_SelectColor)) {
		printf(STR(MSG_CANT_CREATE_CONFIG_OBJ_ERR));
		exit(EXIT_FAILURE);
	    }
	}
    }

    if(proc->pr_CLI) {
	size_t l=16;
	while(1) {
	    progname=MyAllocMem(l);
	    if(!progname)
		break;
	    if(GetProgramName(progname,l))
		break;
	    MyFreeMem(progname);
	    l+=16;
	}
	if(!(rda=ReadArgs(TEMPLATE,args,NULL))) {
	    PrintFault(IoErr(),(char*)STR(MSG_ERROR_HEADER_ERR));
	    exit(EXIT_FAILURE);
	}
	if(PDFFILE) {
	    docdir=DupLock(olddir);
	    CopyStr(&docname,PDFFILE);
	}
    } else {
#if defined(__SASC) || defined(__libnix__)
	size_t l;
	if(_WBenchMsg && _WBenchMsg->sm_NumArgs>=1) {
	    CopyStr(&progname,_WBenchMsg->sm_ArgList[0].wa_Name);
	    if(_WBenchMsg->sm_NumArgs>=2) {
		docdir=DupLock(_WBenchMsg->sm_ArgList[1].wa_Lock);
		CopyStr(&docname,_WBenchMsg->sm_ArgList[1].wa_Name);
	    }
	} else {
	    /* should not happen ? */
	    printf("I don't understand the startup method !?\n");
	    exit(EXIT_FAILURE);
	}
#else
	/* adapt for other compilers/libraries */
	exit(EXIT_FAILURE);
#endif
    }

    progdir=GetProgramDir();

    /* gzip sometimes tries to open "*" for input, */
    /* which opens and locks the stdiowin. */
    /* Let's prevent that: */
    /*if(input=Open("NIL:",MODE_OLDFILE))
	oldinput=SelectInput(input);*/

    if(!docname && open_req(NULL)) {
	UnLock(docdir); /* there is a slight chance that docdir is locked... */
	docdir=Lock(req->fr_Drawer,ACCESS_READ);
	CopyStr(&docname,req->fr_File);
    }
    if(!progname || !docname) {
	UnLock(docdir);
	exit(EXIT_FAILURE);
    }

    parse_tooltypes(progdir,progname);
    parse_tooltypes(docdir,docname);

    load_prefs(config,CONFIG);

    {
	ULONG d=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_ModulesDir);
	if(d)
	    modules_dir=Lock((char*)d,ACCESS_READ);
	else
	    modules_dir=DupLock(progdir);
	CurrentDir(modules_dir);
    }

    ppcmodule=FALSE;
    if(!MODULE) {
	ULONG d=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_Module);
	if(d)
	    CopyStr(&module_arg,(const char *)d);
    }
#ifdef HAVE_POWERUP
    if(MODULE && !NOPUP) {
	BPTR file=Open(MODULE,MODE_OLDFILE);
	if(file) {
	    char buf[4];
	    Read(file,buf,4);
	    if(buf[0]=='\177' && buf[1]=='E' && buf[2]=='L' && buf[3]=='F')
		ppcmodule=TRUE;
	    Close(file);
	}
    }
    if((!MODULE && !NOPUP) || ppcmodule)
	PPCLibBase=OpenLibrary("ppc.library",46);
#endif

    pmodule=modules;
    if(MODULE)
	*pmodule++=MODULE;
    else {
#if defined(__MORPHOS__)
	//TODO: cpu check
	*pmodule++="Apdf_604e.module";
	*pmodule++="Apdf_603e.module";
#else
#  if defined(HAVE_POWERUP)
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
#  endif
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
    if(*pmodule==NULL) {
	printf(STR(MSG_CANT_FIND_MODULE_ERR));
	for(pmodule=modules;*pmodule!=NULL;++pmodule)
	    printf("\t%s\n",*pmodule);
	exit(EXIT_FAILURE);
    }

#ifdef HAVE_POWERUP
    if (PPCLibBase && !elfobject) {
	CloseLibrary(PPCLibBase);
	PPCLibBase=NULL;
    }
#endif

    if(
#ifdef __MORPHOS__
       (seglist && !(sv_init=create_mos_server(&sv,seglist))) ||
#else
#  ifdef HAVE_POWERUP
       (elfobject && !(sv_init=create_ppc_server(&sv,elfobject))) ||
#  endif
       (seglist && !(sv_init=create_m68k_server(&sv,seglist))) ||
#endif
       !(ci=(*sv.create_comm_info)(&sv,sizeof(union msg_max)))) {
	printf(STR(MSG_CANT_CREATE_SERVER_ERR));
	exit(EXIT_FAILURE);
    }

    if(!(io_server=initIo()) || !init(ci,VMAGIC,io_server)) {
	printf(STR(MSG_SERVER_INIT_ERR));
	exit(EXIT_FAILURE);
    }
    startServer(io_server);

    {
	BPTR olddir=CurrentDir(progdir);
	ULONG d=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_DiskObj);
	if(d)
	    diskobj=GetDiskObject((char*)d);
	else
	    diskobj=GetDiskObject(progname);
	CurrentDir(olddir);
    }

    CyberGfxBase=OpenLibrary("cybergraphics.library",41); /* may fail */

    use_prefs(ci,config);

    {
	struct NewMenu *p=menudata;
	while(p->nm_Type!=NM_END) {
	    if(p->nm_Label!=NM_BARLABEL) {
		const char *s=getString((int)p->nm_Label);
		if(s[1]=='\0') {
		    p->nm_CommKey=(STRPTR)s;
		    s+=2;
		}
		p->nm_Label=(STRPTR)s;
	    }
	    ++p;
	}
    }

    App=ApplicationObject,
	MUIA_Application_Title,         "Apdf",
	MUIA_Application_Version,       "$VER: " apdfVerString,
	MUIA_Application_Copyright,     "(c) 1996-2002 Derek B. Noonburg, Emmanuel Lesueur",
	MUIA_Application_Author,        "Derek B. Noonburg, Emmanuel Lesueur",
	MUIA_Application_Description,   STR(MSG_COMMODITY_DESCRIPTION),
	MUIA_Application_Base,          "Apdf",
	MUIA_Application_DiskObject,    diskobj,
	MUIA_Application_HelpFile,      "PROGDIR:Apdf.guide",
	MUIA_Application_Commands,      rexxcmds,

	SubWindow,prefswin=WindowObject,
	    MUIA_Window_Title,STR(MSG_GLOBAL_SETTINGS_WIN),
	    MUIA_Window_ID,MAKE_ID('P','R','F','S'),
	    MUIA_Window_PublicScreen,PUBSCR,
	    MUIA_HelpNode,"glbsettings",
	    MUIA_Window_RootObject,VGroup,
		Child,prefsobj=PrefsObject,
		    End,
		Child,HGroup,
		    Child,saveprefsobj=SimpleButton(STR(MSG_SETTINGS_SAVE_GAD)),
		    Child,useprefsobj=SimpleButton(STR(MSG_SETTINGS_USE_GAD)),
		    Child,cancelprefsobj=SimpleButton(STR(MSG_SETTINGS_CANCEL_GAD)),
		    End,
		End,
	    End,

	SubWindow,logwin=WindowObject,
	    MUIA_Window_Title,STR(MSG_LOG_WIN),
	    MUIA_Window_ID,MAKE_ID('L','O','G','\0'),
	    MUIA_Window_PublicScreen,PUBSCR,
	    MUIA_Window_RootObject,VGroup,
		Child,ListviewObject,
		    MUIA_Listview_Input,FALSE,
		    MUIA_Listview_List,logobj=ListObject,
			ReadListFrame,
			MUIA_List_ConstructHook,MUIV_List_ConstructHook_String,
			MUIA_List_DestructHook,MUIV_List_DestructHook_String,
			End,
		    End,
		Child,HGroup,
		    Child,autoopenobj=ImageObject,
			ImageButtonFrame,
			MUIA_InputMode,MUIV_InputMode_Toggle,
			MUIA_Image_Spec,MUII_CheckMark,
			MUIA_Image_FreeVert,TRUE,
			MUIA_Background,MUII_ButtonBack,
			MUIA_ShowSelState,FALSE,
			MUIA_ControlChar,'a',
			MUIA_ShortHelp,STR(MSG_LOG_AUTO_OPEN_HELP),
			End,
		    Child,KeyLLabel1(KSTR(MSG_LOG_AUTO_OPEN_GAD),KEY(MSG_LOG_AUTO_OPEN_GAD)),
		    Child,RectangleObject,
			MUIA_Weight,300,
			End,
		    Child,clearlogobj=SimpleButton(STR(MSG_LOG_CLEAR_GAD)),
		    End,
		End,
	    End,
	End;

    if(App) {
	if(!initComm(App) /*|| !initNotifier(App)*/)
	    exit(10);

	/* create the main window after, because the Document constructor
	   needs the 'App' object to add the fontmap subwindow */
	DB(kprintf("Start: zoom %d\n",ZOOM+5);)
	win=DocWindowObject,
	    MYATTR_DocWindow_Document,getDocument(ci,App,NULL,docdir,docname),
	    PAGE?MYATTR_DocWindow_PageLabel:TAG_IGNORE,PAGE,
	    ZOOM==-1?TAG_IGNORE:MYATTR_DocWindow_Zoom,ZOOM+5,
	    End;
	if(win)
	    DoMethod3(App,OM_ADDMEMBER,win);
    }

    UnLock(docdir);

    if(!App || !win) {
	if(!App)
	    printf(STR(MSG_CANT_CREATE_MUI_APP_ERR));
	else {
	    char msg[256];
	    while(get_error(ci,msg,sizeof(msg)))
		printf("%s\n",msg);
	    printf(STR(MSG_CANT_LOAD_DOC_ERR));
	}
	exit(EXIT_FAILURE);
    }

    DoMethod9(prefswin,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
	     prefswin,3,MUIM_Set,MUIA_Window_Open,FALSE);
    DoMethod8(saveprefsobj,MUIM_Notify,MUIA_Pressed,FALSE,
	     App,2,MUIM_Application_ReturnID,ID_SAVEPREFS);
    DoMethod8(useprefsobj,MUIM_Notify,MUIA_Selected,FALSE,
	     App,2,MUIM_Application_ReturnID,ID_USEPREFS);
    DoMethod8(cancelprefsobj,MUIM_Notify,MUIA_Pressed,FALSE,
	     App,2,MUIM_Application_ReturnID,ID_CANCELPREFS);
    DoMethod9(logwin,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
	     logwin,3,MUIM_Set,MUIA_Window_Open,FALSE);
    DoMethod8(logobj,MUIM_Notify,MUIA_List_Entries,41,
	     logobj,2,MUIM_List_Remove,MUIV_List_Remove_First);
    DoMethod7(clearlogobj,MUIM_Notify,MUIA_Pressed,FALSE,
	     logobj,1,MUIM_List_Clear);
    DoMethod9(autoopenobj,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
	     logobj,3,MUIM_WriteLong,MUIV_NotTriggerValue,&err_quiet);
    set(App,MUIA_Application_DropObject,win);

    DoMethod3(prefsobj,MUIM_Settingsgroup_ConfigToGadgets,config);
    {
	ULONG d=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_AutoOpenLog);
	err_quiet=d?!*(int*)d:0;
	set(autoopenobj,MUIA_Selected,!err_quiet);
    }

    set(win,MUIA_Window_Open,TRUE);
    {
	LONG x;
	get(win,MUIA_Window_Open,&x);
	if(!x) {
	    printf("Can't open window.\n");
	    exit(EXIT_FAILURE);
	}
    }

    sigs=0;
    running=TRUE;
    do {
	do {
	    LONG id=DoMethod3(App,MUIM_Application_NewInput,&sigs);
	    switch(id) {
	      case MUIV_Application_ReturnID_Quit:
		running=FALSE;
		break;

	      case ID_ABOUT: {
		APTR state;
		struct List *list;
		Object* w;
		int opened=FALSE;
		const char *params[3],*fmt;
		get(App,MUIA_Application_WindowList,&list);
		state=list->lh_Head;
		while(!opened && (w=NextObject(&state)))
		    get(w,MUIA_Window_Open,&opened);
		params[0]=STR(MSG_USED_MODULE);
		params[1]=*pmodule;
		params[2]=get_version(ci);
		fmt=MUIX_C
		    "Apdf "apdfVersion
#ifdef __MORPHOS__
		    " for MorphOS"
#endif
		    "\n%s \"%s\"\n\n"
		    "%s";
		MUI_RequestA(App,w,0,"Apdf "apdfVersion,
			     (char*)STR(MSG_REQ_OK),
			     (char*)fmt,(APTR)params);
		break;
	      }

	      case ID_DELETE:
		if(toDelete) {
		    set(toDelete,MUIA_Window_Open,FALSE);
		    DoMethod3(App,OM_REMMEMBER,toDelete);
		    MUI_DisposeObject(toDelete);
		    toDelete=NULL;
		    if(win_count==0)
			running=FALSE;
		}
		break;

	      case ID_SAVEPREFS:
	      case ID_USEPREFS:
		DoMethod3(prefsobj,MUIM_Settingsgroup_GadgetsToConfig,config);
		if(id==ID_SAVEPREFS)
		    save_prefs(config,CONFIG);
		set(App,MUIA_Application_Iconified,TRUE);
		set(prefswin,MUIA_Window_Open,FALSE);
		use_prefs(ci,config);
		set(App,MUIA_Application_Iconified,FALSE);
		break;

	      case ID_CANCELPREFS:
		set(prefswin,MUIA_Window_Open,FALSE);
		DoMethod3(prefsobj,MUIM_Settingsgroup_ConfigToGadgets,config);
		break;

	      /*case ID_LASTSAVEDPREFS:
		load_prefs(config,CONFIG);
		DoMethod3(prefsobj,MUIM_Settingsgroup_ConfigToGadgets,config);
		use_prefs(ci,config);
		break;*/
	    }
	} while(running && sigs==0);

	if(running) {
	    if(has_error()) {
		char msg[256];
		BOOL err=FALSE;
		set(logobj,MUIA_List_Quiet,TRUE);
		while(get_error(ci,msg,sizeof(msg))) {
		    DoMethod4(logobj,MUIM_List_InsertSingle,msg,MUIV_List_Insert_Bottom);
		    err=TRUE;
		}
		DoMethod3(logobj,MUIM_List_Jump,MUIV_List_Jump_Bottom);
		set(logobj,MUIA_List_Quiet,FALSE);
		if(err && !err_quiet)
		    set(logwin,MUIA_Window_Open,TRUE);
	    }
	    sigs=Wait(sigs|SIGBREAKF_CTRL_C);
	}

	if(sigs&SIGBREAKF_CTRL_C)
	    running=FALSE;

    } while(running);

    return EXIT_SUCCESS;
}


