/*************************************************************************\
 *                                                                       *
 * prefs.c                                                               *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libraries/asl.h>
#include <libraries/iffparse.h>
#include <libraries/mui.h>
#include <dos/dostags.h>
#include <exec/nodes.h>
#include <exec/execbase.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iffparse.h>
#include <proto/locale.h>
#include "AComm.h"
#include "Apdf.h"
#include "prefs.h"
#include "fontmap.h"
#include "docbitmap.h"
#define CATCOMP_ARRAY
#include "msgs.h"

#define DB(x)   x
#define Static  static

#if HAS_AA
#   define AA(x,y,z) x,y,z
#else
#   define AA(x,y,z)
#endif

int update_time=500;
int make_bitmap;
int spare_mem;

extern struct Catalog *catalog;

const char *getString(int n) {
    int i;

    if(LocaleBase) {
	for (i = 0;  i < sizeof(CatCompArray) / sizeof(CatCompArray[0]); i++) {
	    if (n == CatCompArray[i].cca_ID) {
		return GetCatalogStr(catalog, i, CatCompArray[i].cca_Str);
	    }
	}
    }
    return "";
}

const char *getKString(int n) {
    const char *s=getString(n);
    return s[1]=='\0'?s+2:s;
}

char getKey(int n) {
    const char *s=getString(n);
    return s[1]=='\0'?s[0]:'\0';
}


static struct MUI_CustomClass* cache_slider_mcc;
#define CacheSliderObject NewObject(cache_slider_mcc->mcc_Class,NULL

struct CacheSliderData {
    char buf[32];
};
typedef struct CacheSliderData CacheSliderData;

Static ULONG csStringify(struct IClass *cl,Object *obj,
			 struct MUIP_Numeric_Stringify *msg) {
    LONG v=msg->value;
    if(v==129*1024)
	return (ULONG)STR(MSG_PREFS_LOADALL);
    else {
	CacheSliderData *dat=INST_DATA(cl,obj);
	char *p=dat->buf+sizeof(dat->buf);
	*--p='\0';
	*--p='B';
	if(v<1024) {
	    *--p='K';
	} else {
	    *--p='M';
	    *--p='0'+(v/102)%10;
	    *--p='.';
	    v>>=10;
	}
	do {
	    *--p='0'+v%10;
	    v/=10;
	} while(v);
	return (ULONG)p;
    }
}

Static ULONG csScaleToValue(struct IClass *cl,Object *obj,
			    struct MUIP_Numeric_ScaleToValue *msg) {
    LONG r;
    r=(msg->scale-msg->scalemin)*((1024-64>>6)+128)/(msg->scalemax-msg->scalemin);
    if(r<=1024-64>>6)
	r=(r<<6)+64;
    else
	r=((r-(1024-64>>6))<<10)+1024;
    return r;
}

Static ULONG csValueToScale(struct IClass *cl,Object *obj,
			    struct MUIP_Numeric_ValueToScale *msg) {
    LONG val,r;
    get(obj,MUIA_Numeric_Value,&val);
    if(val<=1024)
	r=val-64>>6;
    else
	r=((val-1024)>>10)+(1024-64>>6);
    r*=msg->scalemax-msg->scalemin;
    r/=(1024-64>>6)+128;
    r+=msg->scalemin;
    return r;
}

BEGIN_DISPATCHER(csDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case MUIM_Numeric_Stringify: return csStringify(cl,obj,(APTR)msg);
      case MUIM_Numeric_ScaleToValue: return csScaleToValue(cl,obj,(APTR)msg);
      case MUIM_Numeric_ValueToScale: return csValueToScale(cl,obj,(APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(csDispatcher)

struct PrefsData {
    Object *url_cmd_obj;
    Object *tmpdir_obj;
    Object *print_dev_obj;
    Object *deficon_obj;
    Object *diskobj_obj;
    Object *cache_size_obj;
    Object *cache_bloc_obj;
    Object *colors_obj;
    Object *selectcolor_obj;
    Object *pathlv_obj;
    Object *pop_obj;
    Object *module_obj;
#if HAS_AA
    Object *textaa_obj;
    Object *fillaa_obj;
    Object *strokeaa_obj;
#endif
    Object *autolog_obj;
    Object *fontmap_obj;
    Object *modulesdir_obj;
    Object *zoom_obj;
    Object *continuous_obj;
    Object *dispmode_obj;
    Object *columns_obj;
    Object *rows_obj;
    Object *make_bitmap_obj;
    Object *spare_mem_obj;
    Object *update_time_obj;
    Object *extoutlines_obj;
    Object *openoutlines_obj;
};
typedef struct PrefsData PrefsData;

BEGIN_STATIC_HOOK(ULONG,update,
		  struct Hook *,h,
		  Object *,lv,
		  Object **,strp) {
    Object *str=*strp;
    LONG x;
    get(lv,MUIA_List_Active,&x);
    if(x!=MUIV_List_Active_Off) {
	const char *s;
	get(str,MUIA_String_Contents,&s);
	set(lv,MUIA_List_Quiet,TRUE);
	nnset(lv,MUIA_List_Active,MUIV_List_Active_Off);
	DoMethod4(lv,MUIM_List_InsertSingle,s,x);
	DoMethod3(lv,MUIM_List_Remove,x+1);
	nnset(lv,MUIA_List_Active,x);
	set(lv,MUIA_List_Quiet,FALSE);
    }
    return 0;
}
END_STATIC_HOOK(update)

BEGIN_STATIC_HOOK(ULONG,active,
		  struct Hook *,h,
		  Object *,lv,
		  Object **,objp) {
    Object *str=objp[0];
    Object *rem=objp[1];
    Object *pop=objp[2];
    LONG x;
    get(lv,MUIA_List_Active,&x);
    set(pop,MUIA_Disabled,x==MUIV_List_Active_Off);
    set(rem,MUIA_Disabled,x==MUIV_List_Active_Off);
    if(x!=MUIV_List_Active_Off) {
	const char *s;
	DoMethod4(lv,MUIM_List_GetEntry,x,&s);
	nnset(str,MUIA_String_Contents,s);
	set(_win(lv),MUIA_Window_ActiveObject,str);
    }
    return 0;
}
END_STATIC_HOOK(active)


static char aslreq_buf[272];

BEGIN_STATIC_HOOK(ULONG,open_iconreq,
		  struct Hook *,h,
		  Object *,obj,
		  struct TagItem *,tags) {
    char *str;
    char *p;
    char *file;
    int len;
    while(tags->ti_Tag!=TAG_END)
	++tags;
    get(obj,MUIA_String_Contents,&str);
    if(str) {
	p=PathPart(str);
	memcpy(aslreq_buf,str,p-str);
	file=&aslreq_buf[p-str];
	*file++='\0';
	tags->ti_Tag=ASLFR_InitialDrawer;
	tags->ti_Data=(LONG)aslreq_buf;
	++tags;
	tags->ti_Tag=ASLFR_InitialFile;
	tags->ti_Data=(LONG)file;
	++tags;
	tags->ti_Tag=TAG_DONE;
	p=FilePart(str);
	len=strlen(p);
	memcpy(file,p,len);
	file+=len;
	if(len<5||Stricmp(file-5,".info")) {
	    *file++='.';
	    *file++='i';
	    *file++='n';
	    *file++='f';
	    *file++='o';
	}
	*file='\0';
    }
    return TRUE;
}
END_STATIC_HOOK(open_iconreq)

BEGIN_STATIC_HOOK(void,close_iconreq,
		  struct Hook *,h,
		  Object *,obj,
		  struct FileRequester *,req) {
    int len;
    aslreq_buf[0]='\0';
    if(req->fr_Drawer) {
	strncpy(aslreq_buf,req->fr_Drawer,sizeof(aslreq_buf));
	aslreq_buf[sizeof(aslreq_buf)-1]='\0';
    }
    if(req->fr_File)
	AddPart(aslreq_buf,req->fr_File,sizeof(aslreq_buf));
    len=strlen(aslreq_buf);
    if(len>5 && !Stricmp(&aslreq_buf[len-5],".info"))
	aslreq_buf[len-5]='\0';
    set(obj,MUIA_String_Contents,aslreq_buf);
}
END_STATIC_HOOK(close_iconreq)


const char *zoomEntries[]={
    "-5","-4","-3","-2","-1","0","+1","+2","+3","+4","+5",
    NULL,NULL,NULL
};
const char *dispEntries[4];


Static ULONG prfNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    struct TagItem tags[2];
    Object *url_cmd_obj,*print_dev_obj;
    Object *deficon_obj,*diskobj_obj,*cache_size_obj,*pop_obj;
    Object *cache_bloc_obj,*colors_obj,*selectcolor_obj;
    Object *pathlv_obj,*pathstr_obj,*newpath_obj,*rempath_obj;
    Object *module_obj;
#if HAS_AA
    Object *textaa_obj,*fillaa_obj,*strokeaa_obj;
#endif
    Object *tmpdir_obj,*autolog_obj,*loglab,*loggrp,*extoutlines_obj;
    Object *print_dev_lab,*deficon_lab,*diskobj_lab,*openoutlines_obj;
    Object *fontmap_obj,*modulesdir_obj,*zoom_obj;
    Object *continuous_obj,*dispmode_obj,*columns_obj,*rows_obj;
    Object *make_bitmap_obj,*spare_mem_obj,*update_time_obj;
    static const char *prefstitles[6];
    BOOL plugin=GetTagData(MYATTR_Prefs_Plugin,FALSE,msg->ops_AttrList);
    Object *page=(Object*)GetTagData(MYATTR_Prefs_Page,0,msg->ops_AttrList);
    prefstitles[0]=STR(MSG_PREFS_ABOUT_PAGE);
    prefstitles[1]=STR(MSG_PREFS_GENERAL_PAGE);
    prefstitles[2]=STR(MSG_PREFS_FONT_DIRS_PAGE);
    prefstitles[3]=STR(MSG_PREFS_FONTMAP_PAGE);
    prefstitles[4]=STR(MSG_PREFS_ADVANCED_PAGE);
    if(plugin) {
	url_cmd_obj=NULL;
	tmpdir_obj=PopaslObject,
	    MUIA_Popasl_Type,ASL_FileRequest,
	    MUIA_Popstring_Button,PopButton(MUII_PopDrawer),
	    MUIA_ShortHelp,STR(MSG_PREFS_TMPDIR_HELP),
	    MUIA_Popstring_String,StringObject,
		StringFrame,
		MUIA_String_MaxLen,120,
		MUIA_CycleChain,TRUE,
		End,
	    ASLFR_TitleText,STR(MSG_PREFS_TMPDIR_REQ),
	    ASLFR_DrawersOnly,TRUE,
	    ASLFR_RejectIcons,TRUE,
	    End;
	autolog_obj=NULL;
	deficon_obj=NULL;
	diskobj_obj=NULL;
    } else {
	url_cmd_obj=StringObject,
	    StringFrame,
	    MUIA_CycleChain,TRUE,
	    MUIA_ShortHelp,STR(MSG_PREFS_URLCMD_HELP),
	    End;
	tmpdir_obj=NULL;
	loglab=Label1(STR(MSG_PREFS_AUTO_OPEN_GAD));
	loggrp=HGroup,
	    MUIA_ShortHelp,STR(MSG_PREFS_AUTO_OPEN_HELP),
	    Child,autolog_obj=MUI_MakeObject(MUIO_Checkmark,STR(MSG_PREFS_AUTO_OPEN_GAD)),
	    Child,RectangleObject,
		End,
	    End;
	diskobj_lab=Label2(STR(MSG_PREFS_DISKOBJ_GAD));
	diskobj_obj=PopaslObject,
	    MUIA_Popasl_Type,ASL_FileRequest,
	    MUIA_Popasl_StartHook,&open_iconreq_hook,
	    MUIA_Popasl_StopHook,&close_iconreq_hook,
	    MUIA_Popstring_Button,PopButton(MUII_PopFile),
	    MUIA_ShortHelp,STR(MSG_PREFS_DISKOBJ_HELP),
	    MUIA_Popstring_String,StringObject,
		StringFrame,
		MUIA_String_MaxLen,255,
		MUIA_CycleChain,TRUE,
		MUIA_ShortHelp,STR(MSG_PREFS_DISKOBJ_HELP),
		End,
	    ASLFR_TitleText,STR(MSG_PREFS_ICON_REQ),
	    ASLFR_RejectIcons,FALSE,
	    ASLFR_DoPatterns,TRUE,
	    ASLFR_InitialPattern,"#?.info",
	    End;
	deficon_lab=Label2(STR(MSG_PREFS_DEFICON_GAD));
	deficon_obj=PopaslObject,
	    MUIA_Popasl_Type,ASL_FileRequest,
	    MUIA_Popasl_StartHook,&open_iconreq_hook,
	    MUIA_Popasl_StopHook,&close_iconreq_hook,
	    MUIA_Popstring_Button,PopButton(MUII_PopFile),
	    MUIA_ShortHelp,STR(MSG_PREFS_DISKOBJ_HELP),
	    MUIA_Popstring_String,StringObject,
		StringFrame,
		MUIA_String_MaxLen,255,
		MUIA_CycleChain,TRUE,
		MUIA_ShortHelp,STR(MSG_PREFS_DEFICON_HELP),
		End,
	    ASLFR_TitleText,STR(MSG_PREFS_DEFICON_REQ),
	    ASLFR_RejectIcons,FALSE,
	    ASLFR_DoPatterns,TRUE,
	    ASLFR_InitialPattern,"#?.info",
	    End;
    }

    tags[0].ti_Tag=MUIA_Group_Child;
    tags[0].ti_Data=(ULONG)RegisterObject,
	MUIA_Register_Titles,prefstitles+(page?0:1),
	page?MUIA_Group_Child:TAG_IGNORE,page,
	Child,VGroup,
	    MUIA_HelpNode,"glbprf_general",
	    Child,ColGroup(2),
		Child,Label2(STR(plugin?MSG_PREFS_TMPDIR_GAD:MSG_PREFS_URLCMD_GAD)),
		Child,plugin?tmpdir_obj:url_cmd_obj,
		Child,Label2(STR(MSG_PREFS_PRTDEV_GAD)),
		Child,print_dev_obj=StringObject,
		    StringFrame,
		    MUIA_CycleChain,TRUE,
		    MUIA_ShortHelp,STR(MSG_PREFS_PRTDEV_HELP),
		    End,
		plugin?TAG_IGNORE:MUIA_Group_Child,deficon_lab,
		plugin?TAG_IGNORE:MUIA_Group_Child,deficon_obj,
		Child,Label2(STR(MSG_PREFS_SELECTCOLOR_GAD)),
		Child,HGroup,
		    Child,selectcolor_obj=PoppenObject,
			MUIA_Window_Title,STR(MSG_PREFS_SELECTCOLOR_REQ),
			MUIA_CycleChain,TRUE,
			MUIA_ShortHelp,STR(MSG_PREFS_SELECTCOLOR_HELP),
			End,
		    Child,RectangleObject,
			MUIA_Weight,1000,
			End,
		    End,
		End,
	    Child,ColGroup(2),
		GroupFrame,
		MUIA_FrameTitle,STR(MSG_PREFS_DEFAULT_PARAMS_TITLE),
		Child,Label1(STR(MSG_PREFS_ZOOM_GAD)),
		Child,zoom_obj=CycleObject,
		    MUIA_Cycle_Entries,zoomEntries,
		    MUIA_Cycle_Active,defZoom,
		    MUIA_CycleChain,TRUE,
		    MUIA_ControlChar,'z',
		    MUIA_ShortHelp,STR(MSG_PREFS_ZOOM_HELP),
		    End,
		AA(Child,Label1(STR(MSG_PREFS_ANTIALIASING_GAD)),)
		AA(Child,(HGroup,
		    MUIA_ShortHelp,STR(MSG_PREFS_ANTIALIASING_HELP),
		    Child,textaa_obj=MUI_MakeObject(MUIO_Checkmark,STR(MSG_PREFS_AATEXT_GAD)),
		    Child,LLabel1(STR(MSG_PREFS_AATEXT_GAD)),
		    Child,fillaa_obj=MUI_MakeObject(MUIO_Checkmark,STR(MSG_PREFS_AAFILL_GAD)),
		    Child,LLabel1(STR(MSG_PREFS_AAFILL_GAD)),
		    Child,strokeaa_obj=MUI_MakeObject(MUIO_Checkmark,STR(MSG_PREFS_AASTROKE_GAD)),
		    Child,LLabel1(STR(MSG_PREFS_AASTROKE_GAD)),
		    Child,RectangleObject,
			End,
		    End),)
		plugin?TAG_IGNORE:MUIA_Group_Child,loglab,
		plugin?TAG_IGNORE:MUIA_Group_Child,loggrp,
		Child,Label1(STR(MSG_PREFS_CONTINUOUS_GAD)),
		Child,HGroup,
		    MUIA_ShortHelp,STR(MSG_PREFS_CONTINUOUS_HELP),
		    Child,continuous_obj=MUI_MakeObject(MUIO_Checkmark,STR(MSG_PREFS_CONTINUOUS_GAD)),
		    Child,RectangleObject,
			End,
		    End,
		Child,Label1(STR(MSG_PREFS_EXTOUTLINES_GAD)),
		Child,HGroup,
		    MUIA_ShortHelp,STR(MSG_PREFS_EXTOUTLINES_HELP),
		    Child,extoutlines_obj=MUI_MakeObject(MUIO_Checkmark,STR(MSG_PREFS_EXTOUTLINES_GAD)),
		    Child,RectangleObject,
			End,
		    End,
		Child,Label1(STR(MSG_PREFS_OPENOUTLINES_GAD)),
		Child,HGroup,
		    MUIA_ShortHelp,STR(MSG_PREFS_OPENOUTLINES_HELP),
		    Child,openoutlines_obj=MUI_MakeObject(MUIO_Checkmark,STR(MSG_PREFS_OPENOUTLINES_GAD)),
		    Child,RectangleObject,
			End,
		    End,
		Child,Label1(STR(MSG_PREFS_DISPLAY_MODE_GAD)),
		Child,dispmode_obj=CycleObject,
		    MUIA_Cycle_Entries,dispEntries,
		    MUIA_Cycle_Active,DrPartialUpdates,
		    MUIA_CycleChain,TRUE,
		    MUIA_ShortHelp,STR(MSG_PREFS_DISPLAY_MODE_HELP),
		    End,
		Child,Label1(STR(MSG_PREFS_COLUMNS_GAD)),
		Child,columns_obj=MUI_MakeObject(MUIO_Slider,STR(MSG_PREFS_COLUMNS_GAD),1,10),
		Child,Label1(STR(MSG_PREFS_ROWS_GAD)),
		Child,rows_obj=MUI_MakeObject(MUIO_Slider,STR(MSG_PREFS_ROWS_GAD),1,10),
		End,
	    End,
	Child,VGroup,
	    MUIA_HelpNode,"glbprf_fontdirs",
	    Child,pathlv_obj=ListviewObject,
		MUIA_Listview_Input,TRUE,
		MUIA_Listview_DragType,MUIV_Listview_DragType_Immediate,
		MUIA_Listview_MultiSelect,MUIV_Listview_MultiSelect_Default,
		MUIA_Listview_List,ListObject,
		    InputListFrame,
		    MUIA_List_ConstructHook,MUIV_List_ConstructHook_String,
		    MUIA_List_DestructHook,MUIV_List_DestructHook_String,
		    MUIA_List_DragSortable,TRUE,
		    End,
		MUIA_ShortHelp,STR(MSG_PREFS_FONTDIRS_HELP),
		End,
	    Child,pop_obj=PopaslObject,
		MUIA_Popasl_Type,ASL_FileRequest,
		MUIA_Popstring_Button,PopButton(MUII_PopDrawer),
		MUIA_Popstring_String,pathstr_obj=StringObject,
		    StringFrame,
		    MUIA_String_MaxLen,255,
		    MUIA_CycleChain,TRUE,
		    End,
		MUIA_Disabled,TRUE,
		ASLFR_TitleText,STR(MSG_PREFS_FONTDIR_REQ),
		ASLFR_DrawersOnly,TRUE,
		ASLFR_RejectIcons,TRUE,
		End,
	    Child,HGroup,
		Child,newpath_obj=SimpleButton(STR(MSG_PREFS_FONTDIR_NEW_GAD)),
		Child,rempath_obj=SimpleButton(STR(MSG_PREFS_FONTDIR_REMOVE_GAD)),
		End,
	    End,
	Child,fontmap_obj=FontMapObject,
	    End,
	Child,ColGroup(2),
	    MUIA_HelpNode,"glbprf_advanced",
	    Child,Label2(STR(MSG_PREFS_MODULEDIR_GAD)),
	    Child,PopaslObject,
		MUIA_Popasl_Type,ASL_FileRequest,
		MUIA_Popstring_Button,PopButton(MUII_PopDrawer),
		MUIA_ShortHelp,STR(MSG_PREFS_MODULEDIR_HELP),
		MUIA_Popstring_String,modulesdir_obj=StringObject,
		    StringFrame,
		    MUIA_String_MaxLen,255,
		    MUIA_CycleChain,TRUE,
		    End,
		ASLFR_TitleText,STR(MSG_PREFS_MODULEDIR_REQ),
		ASLFR_RejectIcons,TRUE,
		End,
	    Child,Label2(STR(MSG_PREFS_MODULE_GAD)),
	    Child,PopaslObject,
		MUIA_Popasl_Type,ASL_FileRequest,
		MUIA_Popstring_Button,PopButton(MUII_PopFile),
		MUIA_ShortHelp,STR(MSG_PREFS_MODULE_HELP),
		MUIA_Popstring_String,module_obj=StringObject,
		    StringFrame,
		    MUIA_String_MaxLen,255,
		    MUIA_CycleChain,TRUE,
		    End,
		ASLFR_TitleText,STR(MSG_PREFS_MODULE_REQ),
		ASLFR_RejectIcons,TRUE,
		ASLFR_DoPatterns,TRUE,
		ASLFR_InitialPattern,"#?.module",
		End,
	    plugin?TAG_IGNORE:MUIA_Group_Child,diskobj_lab,
	    plugin?TAG_IGNORE:MUIA_Group_Child,diskobj_obj,
	    Child,Label2(STR(MSG_PREFS_MAXCOLORS_GAD)),
	    Child,colors_obj=SliderObject,
		SliderFrame,
		MUIA_Slider_Horiz,TRUE,
		MUIA_Numeric_Min,2,
		MUIA_Numeric_Max,256,
		MUIA_Numeric_Default,16,
		MUIA_CycleChain,TRUE,
		MUIA_ShortHelp,STR(MSG_PREFS_MAXCOLORS_HELP),
		End,
	    Child,Label2(STR(MSG_PREFS_CACHESZ_GAD)),
	    Child,cache_size_obj=CacheSliderObject,
		SliderFrame,
		MUIA_Slider_Horiz,TRUE,
		MUIA_Numeric_Min,64,
		MUIA_Numeric_Max,129*1024,
		MUIA_Numeric_Default,256,
		MUIA_CycleChain,TRUE,
		MUIA_HelpNode,"cache",
		MUIA_ShortHelp,STR(MSG_PREFS_CACHESZ_HELP),
		End,
	    Child,Label2(STR(MSG_PREFS_CACHEBLOCSZ_GAD)),
	    Child,cache_bloc_obj=SliderObject,
		SliderFrame,
		MUIA_Slider_Horiz,TRUE,
		MUIA_Numeric_Min,1,
		MUIA_Numeric_Max,64,
		MUIA_Numeric_Default,4,
		MUIA_Numeric_Format,"%ldKB",
		MUIA_CycleChain,TRUE,
		MUIA_HelpNode,"cache",
		MUIA_ShortHelp,STR(MSG_PREFS_CACHEBLOCSZ_HELP),
		End,
	    Child,Label2(STR(MSG_PREFS_UPDATE_TIME_GAD)),
	    Child,update_time_obj=SliderObject,
		SliderFrame,
		MUIA_Slider_Horiz,TRUE,
		MUIA_Numeric_Min,1,
		MUIA_Numeric_Max,30,
		MUIA_Numeric_Default,5,
		MUIA_Numeric_Format,"%ld/10s",
		MUIA_CycleChain,TRUE,
		MUIA_ShortHelp,STR(MSG_PREFS_UPDATE_TIME_HELP),
		End,
	    Child,Label1(STR(MSG_PREFS_MAKE_BITMAP_GAD)),
	    Child,HGroup,
		MUIA_ShortHelp,STR(MSG_PREFS_MAKE_BITMAP_HELP),
		Child,make_bitmap_obj=MUI_MakeObject(MUIO_Checkmark,STR(MSG_PREFS_MAKE_BITMAP_GAD)),
		Child,RectangleObject,
		    End,
		End,
	    Child,Label1(STR(MSG_PREFS_SPARE_MEM_GAD)),
	    Child,HGroup,
		MUIA_ShortHelp,STR(MSG_PREFS_SPARE_MEM_HELP),
		Child,spare_mem_obj=MUI_MakeObject(MUIO_Checkmark,STR(MSG_PREFS_SPARE_MEM_GAD)),
		Child,RectangleObject,
		    End,
		End,
	    End,
	End;
    tags[1].ti_Tag=TAG_MORE;
    tags[1].ti_Data=(ULONG)msg->ops_AttrList;
    msg->ops_AttrList=tags;
    if(obj=(Object*)DoSuperMethodA(cl,obj,(Msg)msg)) {
	PrefsData *dat=INST_DATA(cl,obj);
	dat->url_cmd_obj=url_cmd_obj;
	dat->tmpdir_obj=tmpdir_obj;
	dat->print_dev_obj=print_dev_obj;
	dat->deficon_obj=deficon_obj;
	dat->diskobj_obj=diskobj_obj;
	dat->cache_size_obj=cache_size_obj;
	dat->cache_bloc_obj=cache_bloc_obj;
	dat->colors_obj=colors_obj;
	dat->selectcolor_obj=selectcolor_obj;
	dat->pathlv_obj=pathlv_obj;
	dat->pop_obj=pop_obj;
	dat->module_obj=module_obj;
#if HAS_AA
	dat->textaa_obj=textaa_obj;
	dat->fillaa_obj=fillaa_obj;
	dat->strokeaa_obj=strokeaa_obj;
#endif
	dat->autolog_obj=autolog_obj;
	dat->fontmap_obj=fontmap_obj;
	dat->modulesdir_obj=modulesdir_obj;
	dat->zoom_obj=zoom_obj;
	dat->continuous_obj=continuous_obj;
	dat->extoutlines_obj=extoutlines_obj;
	dat->openoutlines_obj=openoutlines_obj;
	dat->dispmode_obj=dispmode_obj;
	dat->columns_obj=columns_obj;
	dat->rows_obj=rows_obj;
	dat->make_bitmap_obj=make_bitmap_obj;
	dat->spare_mem_obj=spare_mem_obj;
	dat->update_time_obj=update_time_obj;
	set(pathstr_obj,MUIA_String_AttachedList,pathlv_obj);
	set(rempath_obj,MUIA_Disabled,TRUE);
	DoMethod8(rempath_obj,MUIM_Notify,MUIA_Pressed,FALSE,
		 pathlv_obj,2,MUIM_List_Remove,MUIV_List_Remove_Selected);
	DoMethod9(newpath_obj,MUIM_Notify,MUIA_Pressed,FALSE,
		 pathlv_obj,3,MUIM_List_InsertSingle,"",MUIV_List_Insert_Bottom);
	DoMethod9(newpath_obj,MUIM_Notify,MUIA_Pressed,FALSE,
		 pathlv_obj,3,MUIM_Set,MUIA_List_Active,MUIV_List_Active_Bottom);
	DoMethod9(pathstr_obj,MUIM_Notify,MUIA_String_Contents,MUIV_EveryTime,
		 pathlv_obj,3,MUIM_CallHook,&update_hook,pathstr_obj);
	DoMethod11(pathlv_obj,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,
		 pathlv_obj,5,MUIM_CallHook,&active_hook,pathstr_obj,rempath_obj,pop_obj);
    }
    msg->ops_AttrList=(struct TagItem *)tags[1].ti_Data;
    return (ULONG)obj;
}

Static ULONG prfConfigToGadgets(struct IClass *cl,Object *obj,
				struct MUIP_Settingsgroup_ConfigToGadgets *msg) {
    PrefsData *dat=INST_DATA(cl,obj);
    Object *dt=msg->configdata;
    ULONG d;
    struct CacheInfo *cache;

    if(dat->url_cmd_obj) {
	/* UrlCmd */
	d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_URLCmd);
	set(dat->url_cmd_obj,MUIA_String_Contents,d?(char*)d:"OpenURL %s");
    } else {
	/* TmpDir */
	d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_TmpDir);
	set(dat->tmpdir_obj,MUIA_String_Contents,(char*)d);
    }

    /* PrintDev */
    if(dat->print_dev_obj) {
	d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_PrintDev);
	set(dat->print_dev_obj,MUIA_String_Contents,d?(char*)d:"PRT:");
    }

    /* DefIcon */
    if(dat->deficon_obj) {
	d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_DefIcon);
	set(dat->deficon_obj,MUIA_String_Contents,d?(char*)d:"ENV:Sys/def_pdf");
    }

    /* DiskObj */
    if(dat->diskobj_obj) {
	d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_DiskObj);
	set(dat->diskobj_obj,MUIA_String_Contents,d?(char*)d:"PROGDIR:Apdf");
    }

    /* Cache */
    cache=(struct CacheInfo *)DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_Cache);
    set(dat->cache_size_obj,MUIA_Numeric_Value,cache?(cache->size?cache->size:129*1024):256);
    set(dat->cache_bloc_obj,MUIA_Numeric_Value,cache?cache->bloc_size:4);

    /* Colors */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_Colors);
    set(dat->colors_obj,MUIA_Numeric_Value,d?*(int*)d:16);

    /* SelectColor */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_SelectColor);
    if(d)
	set(dat->selectcolor_obj,MUIA_Pendisplay_Spec,d);
    else
	DoMethod5(dat->selectcolor_obj,MUIM_Pendisplay_SetRGB,0xffffffff,0,0);

    /* ModulesDir */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_ModulesDir);
    set(dat->modulesdir_obj,MUIA_String_Contents,d);

    /* Module */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_Module);
    set(dat->module_obj,MUIA_String_Contents,d);

    /* Zoom */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_Zoom);
    set(dat->zoom_obj,MUIA_Cycle_Active,d?*(int*)d:defZoom);

#if HAS_AA
    /* Anti-aliasing */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_AntiAliasing);
    d=d?*(int*)d:1;
    set(dat->textaa_obj,MUIA_Selected,d&1);
    set(dat->fillaa_obj,MUIA_Selected,d&256);
    set(dat->strokeaa_obj,MUIA_Selected,d&65536);
#endif

    /* AutoOpenLog */
    if(dat->autolog_obj) {
	d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_AutoOpenLog);
	set(dat->autolog_obj,MUIA_Selected,d?*(int*)d:TRUE);
    }

    /* Continuous */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_Continuous);
    set(dat->continuous_obj,MUIA_Selected,d?*(int*)d:FALSE);

    /* ExtOutlines */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_ExtOutlines);
    set(dat->extoutlines_obj,MUIA_Selected,d?*(int*)d:FALSE);

    /* OpenOutlines */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_OpenOutlines);
    set(dat->openoutlines_obj,MUIA_Selected,d?*(int*)d:FALSE);

    /* DisplayMode */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_DisplayMode);
    set(dat->dispmode_obj,MUIA_Cycle_Active,d?*(int*)d:DrPartialUpdates);

    /* Columns */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_Columns);
    set(dat->columns_obj,MUIA_Numeric_Value,d?*(int*)d:1);

    /* Rows */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_Rows);
    set(dat->rows_obj,MUIA_Numeric_Value,d?*(int*)d:1);

    /* Make bitmap */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_MakeBitMap);
    set(dat->make_bitmap_obj,MUIA_Selected,d?*(int*)d:FALSE);

    /* Spare mem */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_SpareMem);
    set(dat->spare_mem_obj,MUIA_Selected,d?*(int*)d:FALSE);

    /* Update time */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_UpdateTime);
    set(dat->update_time_obj,MUIA_Numeric_Value,d?*(int*)d/100:5);

    /* FontPath */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_FontPath);
    DoMethod2(dat->pathlv_obj,MUIM_List_Clear);
    if(d) {
	int n=*(int*)d;
	int k;
	char *p=(char*)d+sizeof(int);
	for(k=0;k<n;++k) {
	    DoMethod4(dat->pathlv_obj,MUIM_List_InsertSingle,p,MUIV_List_Insert_Bottom);
	    p+=strlen(p)+1;
	}
    }

    /* FontMap */
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_FontMap);
    DoMethod2(dat->fontmap_obj,MYM_FontMap_Clear);
    if(d) {
	int n=*(int*)d;
	int k;
	char *p=(char*)d+sizeof(int);
	for(k=0;k<n;++k) {
	    struct fontmapentry e;
	    strncpy(e.pdffont,p,sizeof(e.pdffont)-1);
	    e.pdffont[sizeof(e.pdffont)-1]='\0';
	    strncpy(e.font,p+=strlen(p)+1,sizeof(e.font)-1);
	    e.font[sizeof(e.font)-1]='\0';
	    p+=strlen(p)+1;
	    e.type=*p=='1'?1:4;
	    p+=2;
	    DoMethod4(dat->fontmap_obj,MYM_FontMap_InsertSingle,&e,MUIV_List_Insert_Bottom);
	}
    }
    return 0;
}


Static ULONG prfGadgetsToConfig(struct IClass *cl,Object *obj,
				struct MUIP_Settingsgroup_ConfigToGadgets *msg) {
    PrefsData *dat=INST_DATA(cl,obj);
    Object *dt=msg->configdata;
    struct CacheInfo cache;
    char *s;
    int n;
    ULONG d;
    int sz;
    int k;

    if(dat->url_cmd_obj) {
	/* UrlCmd */
	get(dat->url_cmd_obj,MUIA_String_Contents,&s);
	if(s && *s)
	    DoMethod5(dt,MUIM_Dataspace_Add,s,strlen(s)+1,MYCFG_Apdf_URLCmd);
	else
	    DoMethod3(dt,MUIM_Dataspace_Remove,MYCFG_Apdf_URLCmd);
    } else {
	/* TmpDir */
	get(dat->tmpdir_obj,MUIA_String_Contents,&s);
	if(s && *s)
	    DoMethod5(dt,MUIM_Dataspace_Add,s,strlen(s)+1,MYCFG_Apdf_TmpDir);
	else
	    DoMethod3(dt,MUIM_Dataspace_Remove,MYCFG_Apdf_TmpDir);
    }

    /* PrintDev */
    if(dat->print_dev_obj) {
	get(dat->print_dev_obj,MUIA_String_Contents,&s);
	if(s && *s)
	    DoMethod5(dt,MUIM_Dataspace_Add,s,strlen(s)+1,MYCFG_Apdf_PrintDev);
	else
	    DoMethod3(dt,MUIM_Dataspace_Remove,MYCFG_Apdf_PrintDev);
    }

    /* DefIcon */
    if(dat->deficon_obj) {
	get(dat->deficon_obj,MUIA_String_Contents,&s);
	if(s && *s)
	    DoMethod5(dt,MUIM_Dataspace_Add,s,strlen(s)+1,MYCFG_Apdf_DefIcon);
	else
	    DoMethod3(dt,MUIM_Dataspace_Remove,MYCFG_Apdf_DefIcon);
    }

    /* DiskObj */
    if(dat->diskobj_obj) {
	get(dat->diskobj_obj,MUIA_String_Contents,&s);
	if(s && *s)
	    DoMethod5(dt,MUIM_Dataspace_Add,s,strlen(s)+1,MYCFG_Apdf_DiskObj);
	else
	    DoMethod3(dt,MUIM_Dataspace_Remove,MYCFG_Apdf_DiskObj);
    }

    /* Cache */
    get(dat->cache_size_obj,MUIA_Numeric_Value,&cache.size);
    get(dat->cache_bloc_obj,MUIA_Numeric_Value,&cache.bloc_size);
    if(cache.size==129*1024)
	cache.size=0;
    DoMethod5(dt,MUIM_Dataspace_Add,&cache,sizeof(cache),MYCFG_Apdf_Cache);

    /* Colors */
    get(dat->colors_obj,MUIA_Numeric_Value,&n);
    DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_Colors);

    /* SelectColor */
    get(dat->selectcolor_obj,MUIA_Pendisplay_Spec,&d);
    if(d)
	DoMethod5(dt,MUIM_Dataspace_Add,d,sizeof(struct MUI_PenSpec),MYCFG_Apdf_SelectColor);

    /* ModulesDir */
    get(dat->modulesdir_obj,MUIA_String_Contents,&s);
    if(s && *s)
	DoMethod5(dt,MUIM_Dataspace_Add,s,strlen(s)+1,MYCFG_Apdf_ModulesDir);
    else
	DoMethod3(dt,MUIM_Dataspace_Remove,MYCFG_Apdf_ModulesDir);

    /* Module */
    get(dat->module_obj,MUIA_String_Contents,&s);
    if(s && *s)
	DoMethod5(dt,MUIM_Dataspace_Add,s,strlen(s)+1,MYCFG_Apdf_Module);
    else
	DoMethod3(dt,MUIM_Dataspace_Remove,MYCFG_Apdf_Module);

    /* Zoom */
    get(dat->zoom_obj,MUIA_Cycle_Active,&n);
    DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_Zoom);

#if HAS_AA
    /* Anti-aliasing */
    n=0;
    get(dat->textaa_obj,MUIA_Selected,&d);
    n|=d?1:0;
    get(dat->fillaa_obj,MUIA_Selected,&d);
    n|=d?256:0;
    get(dat->strokeaa_obj,MUIA_Selected,&d);
    n|=d?65536:0;
    DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_AntiAliasing);
#endif

    /* AutoOpenLog */
    if(dat->autolog_obj) {
	get(dat->autolog_obj,MUIA_Selected,&n);
	DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_AutoOpenLog);
    }

    /* Continous */
    get(dat->continuous_obj,MUIA_Selected,&n);
    DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_Continuous);

    /* ExtOutlines */
    get(dat->extoutlines_obj,MUIA_Selected,&n);
    DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_ExtOutlines);

    /* OpenOutlines */
    get(dat->openoutlines_obj,MUIA_Selected,&n);
    DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_OpenOutlines);

    /* DisplayMode */
    get(dat->dispmode_obj,MUIA_Cycle_Active,&n);
    DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_DisplayMode);

    /* Columns */
    get(dat->columns_obj,MUIA_Numeric_Value,&n);
    DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_Columns);

    /* Rows */
    get(dat->rows_obj,MUIA_Numeric_Value,&n);
    DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_Rows);

    /* Make BitMap */
    get(dat->make_bitmap_obj,MUIA_Selected,&n);
    DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_MakeBitMap);

    /* Spare mem */
    get(dat->spare_mem_obj,MUIA_Selected,&n);
    DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_SpareMem);

    /* Update time */
    get(dat->update_time_obj,MUIA_Numeric_Value,&n);
    n *= 100;
    DoMethod5(dt,MUIM_Dataspace_Add,&n,sizeof(n),MYCFG_Apdf_UpdateTime);

    /* FontPath */
    get(dat->pathlv_obj,MUIA_List_Entries,&n);
    sz=sizeof(int);
    for(k=0;k<n;++k) {
	const char *t;
	DoMethod4(dat->pathlv_obj,MUIM_List_GetEntry,k,&t);
	sz+=strlen(t)+1;
    }
    if(s=MyAllocMem(sz)) {
	char *p=s+sizeof(int);
	*(int*)s=n;
	for(k=0;k<n;++k) {
	    const char *t;
	    DoMethod4(dat->pathlv_obj,MUIM_List_GetEntry,k,&t);
	    strcpy(p,t);
	    p+=strlen(t)+1;
	}
	DoMethod5(dt,MUIM_Dataspace_Add,s,sz,MYCFG_Apdf_FontPath);
	MyFreeMem(s);
    }

    /* FontMap */
    get(dat->fontmap_obj,MYATTR_FontMap_Entries,&n);
    sz=sizeof(int);
    for(k=0;k<n;++k) {
	struct fontmapentry* e;
	DoMethod4(dat->fontmap_obj,MUIM_List_GetEntry,k,&e);
	sz+=strlen(e->pdffont);
	sz+=strlen(e->font);
	sz+=4;
    }
    if(s=MyAllocMem(sz)) {
	char *p=s+sizeof(int);
	*(int*)s=n;
	for(k=0;k<n;++k) {
	    struct fontmapentry* e;
	    size_t l;
	    DoMethod4(dat->fontmap_obj,MUIM_List_GetEntry,k,&e);
	    l=strlen(e->pdffont)+1;
	    memcpy(p,e->pdffont,l);
	    p+=l;
	    l=strlen(e->font)+1;
	    memcpy(p,e->font,l);
	    p+=l;
	    *p++=e->type==4?'T':'1';
	    *p++='\0';
	}
	DoMethod5(config,MUIM_Dataspace_Add,s,sz,MYCFG_Apdf_FontMap);
	MyFreeMem(s);
    }
    return 0;
}


BEGIN_DISPATCHER(prfDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:                       return prfNew      (cl,obj,(APTR)msg);
      case MUIM_Settingsgroup_ConfigToGadgets: return prfConfigToGadgets(cl,obj,(APTR)msg);
      case MUIM_Settingsgroup_GadgetsToConfig: return prfGadgetsToConfig(cl,obj,(APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(prfDispatcher)

struct MUI_CustomClass *prefs_mcc;
static int count;

BOOL createPrefsClass(void) {
    if(count++==0) {
	zoomEntries[11]=STR(MSG_FIT_PAGE);
	zoomEntries[12]=STR(MSG_FIT_WIDTH);
	dispEntries[0]=STR(MSG_PREFS_DISPMODE_ONEPASS);
	dispEntries[1]=STR(MSG_PREFS_DISPMODE_TEXTFIRST);
	dispEntries[2]=STR(MSG_PREFS_DISPMODE_PROGRESSIVE);
	if(createFontMapClass()) {
	    if(cache_slider_mcc=MUI_CreateCustomClass(NULL,MUIC_Slider,NULL,sizeof(CacheSliderData),(APTR)&csDispatcher)) {
		if(prefs_mcc=MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(PrefsData),(APTR)&prfDispatcher))
		    return TRUE;
		MUI_DeleteCustomClass(cache_slider_mcc);
	    }
	    deleteFontMapClass();
	}
	count=0;
	return FALSE;
    }
    return TRUE;
}

BOOL deletePrefsClass(void) {
    if(count && --count==0) {
	MUI_DeleteCustomClass(prefs_mcc);
	MUI_DeleteCustomClass(cache_slider_mcc);
	deleteFontMapClass();
    }
    return TRUE;
}


BOOL save_prefs(Object *dt,const char *filename) {
    struct IFFHandle *iff;
    BOOL ok=FALSE;
    if(iff=AllocIFF()) {
	if(iff->iff_Stream=Open((char*)filename,MODE_NEWFILE)) {
	    InitIFFasDOS(iff);
	    if(!OpenIFF(iff,IFFF_WRITE)) {
		struct PrefHeader hdr={1,0,0};
		if(!PushChunk(iff,ID_PREF,ID_FORM,IFFSIZE_UNKNOWN) &&
		   !PushChunk(iff,0,ID_PRHD,IFFSIZE_UNKNOWN) &&
		   WriteChunkBytes(iff,&hdr,sizeof(hdr))==sizeof(hdr) &&
		   !PopChunk(iff) &&
		   !DoMethod5(dt,MUIM_Dataspace_WriteIFF,iff,0,MAKE_ID('A','P','D','F')) &&
		   !PopChunk(iff))
		    ok=TRUE;
		CloseIFF(iff);
	    }
	    Close(iff->iff_Stream);
	}
	FreeIFF(iff);
    }
    return ok;
}

BOOL load_prefs(Object *dt,const char *filename) {
    struct IFFHandle *iff;
    BOOL ok=FALSE;
    if(iff=AllocIFF()) {
	if(iff->iff_Stream=Open((char*)filename,MODE_OLDFILE)) {
	    InitIFFasDOS(iff);
	    if(!OpenIFF(iff,IFFF_READ)) {
		if(!StopChunk(iff,ID_PREF,MAKE_ID('A','P','D','F'))) {
		    while(TRUE) {
			struct ContextNode *cn;
			LONG err=ParseIFF(iff,IFFPARSE_SCAN);
			if(err==IFFERR_EOC)
			    continue;
			else if(err)
			    break;
			cn=CurrentChunk(iff);
			if(cn && cn->cn_Type==ID_PREF && cn->cn_ID==MAKE_ID('A','P','D','F')) {
			    if(!DoMethod5(dt,MUIM_Dataspace_ReadIFF,iff,ID_PREF,ID_FORM))
				ok=TRUE;
			    break;
			}
		    }
		}
		CloseIFF(iff);
	    }
	    Close(iff->iff_Stream);
	}
	FreeIFF(iff);
    }
    return ok;
}


void use_prefs(struct comm_info *ci,Object *dt) {
    struct msg_prefs *p=ci->cmddata;
    ULONG d;
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_Colors);
    p->colors=d?*(int*)d:16;
    p->gzipcmd[0]='\0';
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_Cache);
    p->cachesz=d?((struct CacheInfo *)d)->size:256;
    p->cacheblocsz=d?((struct CacheInfo *)d)->bloc_size:4;
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_MakeBitMap);
    make_bitmap=d?*(int*)d:FALSE;
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_SpareMem);
    spare_mem=d?*(int*)d:FALSE;
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_UpdateTime);
    update_time=d?*(int*)d:500;
    sync();
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_USEPREFS);
    (*ci->sv->exchange_msg)(ci,NULL,0,MSGID_CLEARFONTPATH);
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_FontPath);
    if(d) {
	int n=*(int*)d;
	int k;
	char *p=(char*)d+sizeof(int);
	for(k=0;k<n;++k) {
	    struct msg_addfontpath *q=ci->cmddata;
	    strncpy(q->path,p,sizeof(q->path)-1);
	    q->path[sizeof(q->path)-1]='\0';
	    (*ci->sv->exchange_msg)(ci,&q->base,sizeof(*q),MSGID_ADDFONTPATH);
	    p+=strlen(p)+1;
	}
    }
    d=DoMethod3(dt,MUIM_Dataspace_Find,MYCFG_Apdf_FontMap);
    clear_fontmap(ci,NULL);
    if(d) {
	int n=*(int*)d;
	int k;
	char *p=(char*)d+sizeof(int);
	for(k=0;k<n;++k) {
	    const char *pdffont=p;
	    const char *font=p+=strlen(p)+1;
	    int type;
	    p+=strlen(p)+1;
	    type=*p=='1'?1:4;
	    p+=2;
	    add_fontmap(ci,NULL,pdffont,font,type);
	}
    }
    refresh_doc_fontmaps();
}

