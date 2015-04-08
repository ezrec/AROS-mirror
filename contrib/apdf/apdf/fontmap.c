/*************************************************************************\
 *                                                                       *
 * fontmap.c                                                             *
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
#include <exec/nodes.h>
#include <exec/execbase.h>
#include <workbench/workbench.h>
#include <proto/muimaster.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include "AComm.h"
#include "Apdf.h"
#include "fontmap.h"
#include "msgs.h"

#define DB(x)   //x
#define Static  static


struct FontMapData {
    Object *obj;
    Object *fontmapobj;
    Object *pdffontobj;
    Object *fontobj;
    Object *popobj;
    Object *remfontobj;
    Object *typeobj;
    int update_disabled;
};
typedef struct FontMapData FontMapData;

Static int get_font(struct comm_info *ci,
		    struct FontOutputDev *out,int n,
		    char *pdffont,int pdffontlen,
		    char *font,int fontlen,int *type,int *flags) {
    struct msg_fontmap* p=ci->cmddata;
    int r;
    sync();
    p->out=out;
    p->m=n;
    r=(*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_GETFONT);
    *type=p->type;
    *flags=p->flags;
    strncpy(pdffont,p->pdffont,pdffontlen-1);
    pdffont[pdffontlen-1]='\0';
    strncpy(font,p->font,fontlen-1);
    font[fontlen-1]='\0';
    return r;
}

void add_fontmap(struct comm_info *ci,
		 struct PDFDoc *pdfdoc,const char* pdffont,
		 const char* font,int type) {
    struct msg_addfontmap* p=ci->cmddata;
    sync();
    p->pdfdoc=pdfdoc;
    strncpy(p->pdffont,pdffont,sizeof(p->pdffont)-1);
    p->pdffont[sizeof(p->pdffont)-1]='\0';
    strncpy(p->font,font,sizeof(p->font)-1);
    p->font[sizeof(p->font)-1]='\0';
    p->type=type;
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_ADDFONTMAP);
}

void clear_fontmap(struct comm_info *ci,struct PDFDoc *pdfdoc) {
    struct msg_addfontmap* p=ci->cmddata;
    sync();
    p->pdfdoc=pdfdoc;
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_CLEARFONTMAP);
}


/*
 * Custom list class: allows drag and drop
 */

#define MYATTR_MyList_ID            (MYTAG_START+101)

static struct MUI_CustomClass* list_mcc;

struct ListData {
    LONG id;
};
typedef struct ListData ListData;

#define MyListObject NewObject(list_mcc->mcc_Class,NULL

Static ULONG mlNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    ListData* dat;

    if(obj=(Object *)DoSuperMethodA(cl,obj,(Msg)msg)) {
	dat=INST_DATA(cl,obj);
	dat->id=GetTagData(MYATTR_MyList_ID,0,msg->ops_AttrList);
    }
    return (ULONG)obj;
}

Static ULONG mlGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    switch(msg->opg_AttrID) {
      case MYATTR_MyList_ID: {
	ListData* dat=INST_DATA(cl,obj);
	*msg->opg_Storage=dat->id;
	return TRUE;
      }
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

Static ULONG mlDragQuery(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg) {
    ListData *dat=INST_DATA(cl,obj);
    LONG x;
    if(msg->obj==obj)
	return DoSuperMethodA(cl,obj,(Msg)msg);
    else if(get(msg->obj,MYATTR_MyList_ID,&x))
	return MUIV_DragQuery_Accept;
    else
	return MUIV_DragQuery_Refuse;
}


Static ULONG mlDragDrop(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg) {
    if(msg->obj==obj)
	return DoSuperMethodA(cl,obj,(Msg)msg);
    else {
	LONG dropmark;
	LONG id=MUIV_List_NextSelected_Start;
	LONG listid;
	get(obj,MUIA_List_DropMark,&dropmark);
	get(msg->obj,MYATTR_MyList_ID,&listid);
	while(1) {
	    APTR entry;
	    struct fontmapentry e;
	    DoMethod3(msg->obj,MUIM_List_NextSelected,&id);
	    if(id==MUIV_List_NextSelected_End)
		break;
	    DoMethod4(msg->obj,MUIM_List_GetEntry,id,&entry);
	    if(listid==2) {
		e=*(struct fontmapentry *)entry;
		e.font[0]='\0';
		e.type=1;
		entry=&e;
	    }
	    DoMethod4(obj,MUIM_List_InsertSingle,entry,dropmark);
	}
	if(listid!=2) {
	    DoMethod3(msg->obj,MUIM_List_Remove,MUIV_List_Remove_Selected);
	    set(msg->obj,MUIA_List_Active,MUIV_List_Active_Off);
	}
	get(obj,MUIA_List_InsertPosition,&dropmark);
	set(obj,MUIA_List_Active,dropmark);
	return 0;
    }
}


BEGIN_DISPATCHER(mlDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:         return mlNew      (cl,obj,(APTR)msg);
      case OM_GET:         return mlGet      (cl,obj,(APTR)msg);
      case MUIM_DragQuery: return mlDragQuery(cl,obj,(APTR)msg);
      case MUIM_DragDrop : return mlDragDrop (cl,obj,(APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(mlDispatcher)

/*
 *  Hooks.
 */


BEGIN_STATIC_HOOK(APTR,fontmapctor,
		  struct Hook *,h,
		  APTR,pool,
		  struct fontmapentry *,p) {
    struct fontmapentry* q=AllocPooled(pool,sizeof(*q));
    if(q)
	*q=*p;
    return q;
}
END_STATIC_HOOK(fontmapctor)


BEGIN_STATIC_HOOK(APTR,fontmapdtor,
		  struct Hook *,h,
		  APTR,pool,
		  struct fontmapentry *,p) {
    FreePooled(pool,p,sizeof(*p));
}
END_STATIC_HOOK(fontmapdtor)


static const char *types[]={
    "Unknown","Type1","Type1C","Type3","TrueType","Type0", "Type0C", "Type2"
};
static const char *fontmap_titles[3];

BEGIN_STATIC_HOOK(APTR,fontmapdisp,
		  struct Hook *,h,
		  const char **,array,
		  struct fontmapentry *,p) {
    if(p) {
	*array++=p->pdffont;
	*array++=p->font;
	*array++=types[p->type>=sizeof(types)/sizeof(*types) || p->type<0 ? 0:p->type];
    } else {
	*array++=fontmap_titles[0];
	*array++=fontmap_titles[1];
	*array++=fontmap_titles[2];
    }
    return 0;
}
END_STATIC_HOOK(fontmapdisp)


static char last_drawer[128];

BEGIN_STATIC_HOOK(ULONG,popaslstart,
		  struct Hook *,h,
		  Object *,obj,
		  struct TagItem *,tags) {
    tags[5].ti_Tag=ASLFR_InitialDrawer;
    tags[5].ti_Data=(ULONG)last_drawer;
    tags[6].ti_Tag=ASLFR_InitialFile;
    get(obj,MUIA_String_Contents,&tags[6].ti_Data);
    tags[7].ti_Tag=TAG_END;
    return 1;
}
END_STATIC_HOOK(popaslstart)

BEGIN_STATIC_HOOK(ULONG,popaslstop,
		  struct Hook *,h,
		  Object *,obj,
		  struct FileRequester *,req) {

    // copy filename but without extension
    char tempstr[108];
    int pos;

    stccpy(tempstr, req->fr_File, sizeof(tempstr));
    pos = strlen(tempstr) - 1;
    while(pos >= 0 && tempstr[pos] != '.')
      pos--;

    if (pos > 0)
      tempstr[pos] = '\0';

    set(obj,MUIA_String_Contents,tempstr);
    strncpy(last_drawer,req->fr_Drawer,sizeof(last_drawer)-1);
    last_drawer[sizeof(last_drawer)-1]='\0';

}
END_STATIC_HOOK(popaslstop)

static const char* fontinfo_titles[4];
BEGIN_STATIC_HOOK(APTR,fontinfodisp,
		  struct Hook *,h,
		  const char **,array,
		  struct fontmapentry *,p) {
    if(p) {
	*array++=p->pdffont;
	*array++=types[p->type>=sizeof(types)/sizeof(*types) || p->type<0 ? 0:p->type];
	*array++=p->flags?"\33c×":"";
	*array++=p->font;
    } else {
	*array++=fontinfo_titles[0];
	*array++=fontinfo_titles[1];
	*array++=fontinfo_titles[2];
	*array++=fontinfo_titles[3];
    }
    return 0;
}
END_STATIC_HOOK(fontinfodisp)


/*
 *  Methods.
 */

Static ULONG fmNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    FontMapData* dat;
    struct TagItem tags[2];
    Object *addfontobj,*fontmapobj;
    Object *remfontobj,*pdffontobj,*popobj,*fontobj,*typeobj;
    static const char *typeentries[]={"Type1","TrueType",NULL};

    fontmapobj=ListviewObject,
	MUIA_HelpNode,"fontmap",
	MUIA_ShortHelp,STR(MSG_FONTMAP_HELP),
	MUIA_Listview_Input,TRUE,
	MUIA_Listview_DragType,MUIV_Listview_DragType_Immediate,
	MUIA_Listview_MultiSelect,MUIV_Listview_MultiSelect_Default,
	MUIA_Listview_List,MyListObject,
	    InputListFrame,
	    MUIA_List_Title,TRUE,
	    MUIA_List_Format,"BAR,BAR,",
	    MUIA_List_ConstructHook,&fontmapctor_hook,
	    MUIA_List_DestructHook,&fontmapdtor_hook,
	    MUIA_List_DisplayHook,&fontmapdisp_hook,
	    MUIA_List_DragSortable,TRUE,
	    MUIA_List_ShowDropMarks,TRUE,
	    MYATTR_MyList_ID,2,
	    End,
	End;

    tags[0].ti_Tag=MUIA_Group_Child;
    tags[0].ti_Data=(ULONG)VGroup,
	Child,fontmapobj,
	Child,HGroup,
	    Child,addfontobj=SimpleButton(STR(MSG_FONTMAP_ADD_GAD)),
	    Child,remfontobj=SimpleButton(STR(MSG_FONTMAP_REMOVE_GAD)),
	    End,
	Child,ColGroup(2),
	    Child,Label2(STR(MSG_FONTMAP_PDFFONT_GAD)),
	    Child,pdffontobj=StringObject,
		StringFrame,
		MUIA_String_AdvanceOnCR,TRUE,
		MUIA_String_MaxLen,sizeof(((struct fontmapentry*)NULL)->pdffont),
		MUIA_String_AttachedList,fontmapobj,
		MUIA_CycleChain,TRUE,
		MUIA_Disabled,TRUE,
		End,
	    Child,Label2(STR(MSG_FONTMAP_FONT_GAD)),
	    Child,popobj=PopaslObject,
		MUIA_Popasl_Type,ASL_FileRequest,
		MUIA_Popasl_StartHook,&popaslstart_hook,
		MUIA_Popasl_StopHook,&popaslstop_hook,
		MUIA_Popstring_String,fontobj=StringObject,
		    StringFrame,
		    MUIA_String_MaxLen,sizeof(((struct fontmapentry*)NULL)->font)-5,
		    MUIA_String_AdvanceOnCR,TRUE,
		    MUIA_String_AttachedList,fontmapobj,
		    MUIA_CycleChain,TRUE,
		    MUIA_Disabled,TRUE,
		    End,
		MUIA_Popstring_Button,PopButton(MUII_PopUp),
		ASLFR_TitleText,STR(MSG_FONTMAP_SELECT_FONT_REQ),
		ASLFR_RejectIcons,TRUE,
		ASLFR_DoPatterns,TRUE,
		ASLFR_InitialPattern,"#?.(pfa|pfb|ttf)",
		End,
	    Child,Label1(STR(MSG_FONTMAP_TYPE_GAD)),
	    Child,typeobj=MUI_MakeObject(MUIO_Cycle,STR(MSG_FONTMAP_TYPE_GAD),typeentries),
	    End,
	End;

    tags[1].ti_Tag=TAG_MORE;
    tags[1].ti_Data=(ULONG)msg->ops_AttrList;
    msg->ops_AttrList=tags;

    if(obj=(Object *)DoSuperMethodA(cl,obj,(Msg)msg)) {
	dat=INST_DATA(cl,obj);
	dat->obj=obj;
	dat->fontmapobj=fontmapobj;
	dat->remfontobj=remfontobj;
	dat->pdffontobj=pdffontobj;
	dat->fontobj=fontobj;
	dat->popobj=popobj;
	dat->typeobj=typeobj;
	dat->update_disabled=0;

	DoMethod8(obj,MUIM_MultiSet,MUIA_Disabled,TRUE,
		 remfontobj,pdffontobj,popobj,NULL);
	DoMethod8(fontmapobj,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,
		 obj,2,MYM_FontMap_Activate,MUIV_TriggerValue);
	DoMethod7(pdffontobj,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
		 obj,1,MYM_FontMap_Update);
	DoMethod7(pdffontobj,MUIM_Notify,MUIA_String_Contents,MUIV_EveryTime,
		 obj,1,MYM_FontMap_Update);
	DoMethod7(fontobj,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
		 obj,1,MYM_FontMap_Update);
	DoMethod7(fontobj,MUIM_Notify,MUIA_String_Contents,MUIV_EveryTime,
		 obj,1,MYM_FontMap_Update);
	DoMethod7(typeobj,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,
		 obj,1,MYM_FontMap_Update);
	DoMethod7(addfontobj,MUIM_Notify,MUIA_Pressed,FALSE,
		 obj,1,MYM_FontMap_Add);
	DoMethod8(remfontobj,MUIM_Notify,MUIA_Pressed,FALSE,
		 fontmapobj,2,MUIM_List_Remove,MUIV_List_Remove_Selected);
	set(typeobj,MUIA_Disabled,TRUE);
    }
    msg->ops_AttrList=(struct TagItem *)tags[1].ti_Data;
    return (ULONG)obj;
}

/*Static ULONG fmDispose(struct IClass *cl,Object *obj,Msg msg) {
    FontMapData *dat=INST_DATA(cl,obj);
    return DoSuperMethodA(cl,obj,msg);
}*/

/*Static ULONG fmGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    switch(msg->opg_AttrID) {
      case MYATTR_FontMap_Changed:
	*msg->opg_Storage=FALSE;
	return TRUE;
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}*/

Static ULONG fmActivate(struct IClass* cl,Object* win,struct MYP_FontMap_Activate *msg) {
    FontMapData* dat=INST_DATA(cl,win);
    DoMethod9(win,MUIM_MultiSet,MUIA_Disabled,msg->num==MUIV_List_Active_Off,
	     dat->pdffontobj,dat->popobj,dat->typeobj,dat->remfontobj,NULL);
    if(msg->num!=MUIV_List_Active_Off) {
	struct fontmapentry* e;
	++dat->update_disabled;
	DoMethod4(dat->fontmapobj,MUIM_List_GetEntry,msg->num,&e);
	if(e) {
	    nnset(dat->pdffontobj,MUIA_String_Contents,e->pdffont);
	    nnset(dat->fontobj,MUIA_String_Contents,e->font);
	    nnset(dat->typeobj,MUIA_Cycle_Active,e->type==4?1:0);
	}
	--dat->update_disabled;
    }
    return 0;
}

Static ULONG fmUpdate(struct IClass *cl,Object* obj) {
    FontMapData* dat=INST_DATA(cl,obj);
    LONG x;
    if(dat->update_disabled)
	return 0;
    get(dat->fontmapobj,MUIA_List_Active,&x);
    if(x!=MUIV_List_Active_Off) {
	struct fontmapentry* e;
	const char* p;
	LONG type;
	DoMethod4(dat->fontmapobj,MUIM_List_GetEntry,x,&e);
	if(e) {
	    get(dat->pdffontobj,MUIA_String_Contents,&p);
	    strcpy(e->pdffont,p);
	    get(dat->fontobj,MUIA_String_Contents,&p);
	    strcpy(e->font,p);
	    get(dat->typeobj,MUIA_Cycle_Active,&type);
	    e->type=type?4:1;
	    DoMethod3(dat->fontmapobj,MUIM_List_Redraw,MUIV_List_Redraw_Active);
	}
    }
    return 0;
}

Static ULONG fmAdd(struct IClass *cl,Object *obj) {
    FontMapData *dat=INST_DATA(cl,obj);
    struct fontmapentry e;
    e.pdffont[0]='\0';
    e.font[0]='\0';
    e.type=1;
    DoMethod4(dat->fontmapobj,MUIM_List_InsertSingle,&e,MUIV_List_Insert_Bottom);
    set(dat->fontmapobj,MUIA_List_Active,MUIV_List_Active_Bottom);
    set(obj,MUIA_Window_ActiveObject,dat->pdffontobj);
    return 0;
}

BEGIN_DISPATCHER(fmDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:                      return fmNew     (cl,obj,(APTR)msg);
      /*case OM_DISPOSE:                  return fmDispose (cl,obj,msg);
      case OM_GET:                      return fmGet     (cl,obj,(APTR)msg);*/
      /*case OM_SET:                      return fmSet     (cl,obj,(APTR)msg);*/
      case MYM_FontMap_Activate:  return fmActivate(cl,obj,(APTR)msg);
      case MYM_FontMap_Update:    return fmUpdate  (cl,obj);
      case MYM_FontMap_Add:       return fmAdd     (cl,obj);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(fmDispatcher)

/*
 *  FontInfoLV
 */

struct FontInfoLVData {
    struct comm_info *comm_info;
};
typedef struct FontInfoLVData FontInfoLVData;

Static ULONG fiNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    FontInfoLVData *dat;
    struct TagItem tags[6];
    struct comm_info *ci=(struct comm_info *)GetTagData(MYATTR_FontInfoLV_CommInfo,0,msg->ops_AttrList);

    if(!ci)
	return 0;

    tags[0].ti_Tag=MUIA_Listview_Input;
    tags[0].ti_Data=TRUE;
    tags[1].ti_Tag=MUIA_Listview_DragType;
    tags[1].ti_Data=MUIV_Listview_DragType_Immediate;
    tags[2].ti_Tag=MUIA_Listview_MultiSelect;
    tags[2].ti_Data=MUIV_Listview_MultiSelect_Default;
    tags[3].ti_Tag=MUIA_Listview_List;
    tags[3].ti_Data=(ULONG)MyListObject,
	InputListFrame,
	MUIA_List_Title,TRUE,
	MUIA_List_Format,"BAR,BAR,BAR,",
	MUIA_List_ConstructHook,&fontmapctor_hook,
	MUIA_List_DestructHook,&fontmapdtor_hook,
	MUIA_List_DisplayHook,&fontinfodisp_hook,
	MUIA_Dropable,FALSE,
	MYATTR_MyList_ID,2,
	End;
    tags[4].ti_Tag=MUIA_ShortHelp;
    tags[4].ti_Data=(ULONG)STR(MSG_FONTINFO_HELP);
    tags[5].ti_Tag=TAG_MORE;
    tags[5].ti_Data=(ULONG)msg->ops_AttrList;
    msg->ops_AttrList=tags;

    if(obj=(Object *)DoSuperMethodA(cl,obj,(Msg)msg)) {
	dat=INST_DATA(cl,obj);
	dat->comm_info=ci;
    }
    msg->ops_AttrList=(struct TagItem *)tags[5].ti_Data;
    return (ULONG)obj;
}

Static ULONG fiAddDevContents(struct IClass *cl,Object *obj,
			      struct MYP_FontMap_AddDevContents *msg) {
    FontInfoLVData *dat=INST_DATA(cl,obj);
    if(msg->num) {
	int k;
	set(obj,MUIA_List_Quiet,TRUE);
	for(k=0;k<msg->num;++k) {
	    struct fontmapentry e;
	    if(get_font(dat->comm_info,msg->out,k,e.pdffont,sizeof(e.pdffont),e.font,sizeof(e.font),&e.type,&e.flags)) {
		DoMethod4(obj,MUIM_List_InsertSingle,&e,MUIV_List_Insert_Bottom);
	    }
	}
	set(obj,MUIA_List_Quiet,FALSE);
    }
    return 0;
}


BEGIN_DISPATCHER(fiDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:                      return fiNew     (cl,obj,(APTR)msg);
      /*case OM_DISPOSE:                  return fmDispose (cl,obj,msg);*/
      /*case OM_GET:                      return fmGet     (cl,obj,(APTR)msg);*/
      /*case OM_SET:                      return fmSet     (cl,obj,(APTR)msg);*/
      case MYM_FontInfoLV_AddDevContents: return fiAddDevContents(cl,obj,(APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(fiDispatcher)

struct MUI_CustomClass *FontMap_mcc;
struct MUI_CustomClass *fontinfolv_mcc;
static int count;

BOOL createFontMapClass(void) {
    if(count++==0) {
	fontmap_titles[0]=STR(MSG_FONTMAP_PDFFONT);
	fontmap_titles[1]=STR(MSG_FONTMAP_SUBSTITUTED_FONT);
	fontmap_titles[2]=STR(MSG_FONTMAP_TYPE);
	fontinfo_titles[0]=STR(MSG_FONTINFO_PDFFONT);
	fontinfo_titles[1]=STR(MSG_FONTINFO_TYPE);
	fontinfo_titles[2]=STR(MSG_FONTINFO_EMB);
	fontinfo_titles[3]=STR(MSG_FONTINFO_PROPERTIES);
	if(list_mcc=MUI_CreateCustomClass(NULL,MUIC_List,NULL,sizeof(ListData),(APTR)&mlDispatcher)) {
	    if(fontinfolv_mcc=MUI_CreateCustomClass(NULL,MUIC_Listview,NULL,sizeof(FontInfoLVData),(APTR)&fiDispatcher)) {
		if(FontMap_mcc=MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(FontMapData),(APTR)&fmDispatcher))
		    return TRUE;
		MUI_DeleteCustomClass(fontinfolv_mcc);
	    }
	    MUI_DeleteCustomClass(list_mcc);
	}
	count=0;
	return FALSE;
    }
    return TRUE;
}

BOOL deleteFontMapClass(void) {
    if(count && --count==0) {
	MUI_DeleteCustomClass(FontMap_mcc);
	MUI_DeleteCustomClass(fontinfolv_mcc);
	MUI_DeleteCustomClass(list_mcc);
    }
    return TRUE;
}

