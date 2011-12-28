/********************************************************************

Amiga specific code for controlling the gadget, etc.

*********************************************************************/
/* We really don't need _all_ these... try to narrow it down some time */

#include "includes.h"
#include "HTML.h"
#include "mosaic.h"
#include "gui.h"
#include "globals.h"
#include "XtoI.h"
#include "HTMLP.h"
#include "HTMLImgMacro.h"
#include "htmlgad.h"	/* defines for attributes */
#include "NewGroup.h"
#include "TextEditField.h"
#include "protos.h"
#include <unistd.h>

extern char *cached_url;
extern int lib_version;

/* local functions */

IPTR dispatchHTMLGad(Class *, Object *, Msg);
IPTR RenderHTMLGad(Class *, struct Gadget *, struct gpRender *);
struct Region *installClipRegion(struct Window *, struct Layer *,
				 struct Region *);
void NotifyChange(Class *, Object *, struct gpInput *, Tag,IPTR);
void HTML_reset_search(Widget);
void new_button_press(HTMLGadClData *HTML_Data,long x,long y);
void new_button_release(HTMLGadClData *HTML_Data,Object *obj,long x,long y);
void ImageSubmitForm(FormInfo *,char *,int,int);

#ifdef ZZ

int FormatAll(HTMLPart *, int);
char *ParseTextToString(struct ele_rec *, struct ele_rec *, struct ele_rec *,
			int, int, int, int);
HTMLSetText(HTMLGadClData *, char *, char *, char *, int, char *, void *);
int DocumentWidth(HTMLGadClData *, struct mark_up *);


extern ImageInfo *NoImageData(HTMLGadClData *);
extern void TextRefresh(HTMLGadClData *, struct ele_rec *, int, int);
extern void ImageRefresh(HTMLGadClData *, struct ele_rec *);
extern int SwapElements(struct ele_rec *, struct ele_rec *, int, int);
extern struct ref_rec *AddHRef(struct ref_rec *, char *);
#endif

extern struct mark_up *HTMLParse(struct mark_up *, char *);
extern int DocumentWidth(HTMLGadClData *, struct mark_up *);
extern void ReformatWindow(HTMLGadClData *);
extern void ScrollToPos(HTMLGadClData *, int, int);
extern void ViewClearAndRefresh(HTMLGadClData *);
extern struct ele_rec *LocateElement(HTMLGadClData *, int, int, int *);
extern void SetAnchor(HTMLGadClData *);
extern int IsDelayedHRef(HTMLGadClData *, char *);
extern int AnchoredHeight(HTMLGadClData *);
extern void UnsetAnchor(HTMLGadClData *);
extern void ImageSubmitForm();
extern long IsIsMapForm(HTMLGadClData *,char *);


#define DEBUG0 0 /* Debug the forms/virtual group handlin */
#define DEBUG1 0 /* Debug the image decoding */
#define DEBUG2 0 /* Debug the forms handling */

extern IPTR HookEntry();
extern mo_window window;

extern char *DBGstr; /* mjw*/

extern AppData Rdata;

/* Some compactness typedefs */
typedef struct opSet * opSetP;
typedef struct opGet * opGetP;
typedef struct TextFont * TextFontP;

/************ MISC *************/

#define mui_version (MUIMasterBase->lib_Version)
#define mui_add(x) (mui_version < 11 ? (x) : 0)


#define IEQUALIFIER_SHIFT (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)
#define IEQUALIFIER_ALT (IEQUALIFIER_LALT | IEQUALIFIER_RALT)

#define HIKEY_SHIFT	(0x100)
#define HIKEY_ALT	(0x200)
#define HIKEY_CTRL	(0x400)

#ifndef MUIA_FillArea
#define MUIA_FillArea 0x804294A3 /* V4 BOOL (private) */
#endif
#define MUIV_Listview_MultiSelect_Def 1

/********* CLASS HANDLING *************/

/************************************************************
  Initilize the new HTML MUI Gadget Class (was initHTMLGadClass)
************************************************************/
struct MUI_CustomClass *HTMLText;
struct MUI_CustomClass *HTMLTextN;
extern struct Hook LayoutHook;
extern struct Hook ScrollHook;
extern struct Hook FormButton_hook;
extern struct Hook FormString_hook;
extern struct Hook FormRadio_hook;
extern struct Hook TextEdit_hook;

#ifndef USE_ZUNE
IPTR __stdargs DoSuperNew(struct IClass *cl,Object *obj,IPTR tag1,...)
{
	return(DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL));
}
#endif

/*************** INSTANCE HANDLING (METHODS) *************/
/*
** AskMinMax method will be called before the window is opened
** and before layout takes place. We need to tell MUI the
** minimum, maximum and default size of our object.
*/

static IPTR HTMLGadClAskMinMax(Class *cl, Object *obj, struct MUIP_AskMinMax *msg){
  /*
   ** let our superclass first fill in what it thinks about sizes.
   ** this will e.g. add the size of frame and inner spacing.
   */
  DoSuperMethodA(cl,obj,(Msg)msg);
  /*
   ** now add the values specific to our object. note that we
   ** indeed need to *add* these values, not just set them!
   */
  
#ifndef _HTMLFORMS
  msg->MinMaxInfo->MinWidth  += 100;
  msg->MinMaxInfo->DefWidth  += 300;
  msg->MinMaxInfo->MaxWidth  += 1000;  /* There is an unlimited val, use it*/

  msg->MinMaxInfo->MinHeight += 50;
  msg->MinMaxInfo->DefHeight += 350;
  msg->MinMaxInfo->MaxHeight += 1000; /* same here */
#endif
  return(0);
}

static IPTR HTMLGadClNew(Class *cl, Object *obj,struct opSet *msg){
/* CHECK LAST PARAM */
   
	HTMLGadClData *inst; 

	if (!(obj= (Object *)DoSuperNew(cl,obj,
				MUIA_Group_PageMode,TRUE,
				MUIA_Group_ActivePage,0,
				MUIA_FillArea,FALSE, /* I am handling the background */
						/* If it is a normal page */
				Child,G_Norm = NewObject(HTMLTextN->mcc_Class,NULL,MUIA_FillArea,FALSE,TAG_DONE),
						/* If it is a page with forms */
				Child,G_Virt=VirtgroupObject,
					NoFrame,
//					MUIA_FillArea,FALSE,
					MUIA_Background,MUII_BACKGROUND,
					Child,G_Forms=NewObject(HTMLText->mcc_Class,NULL,MUIA_Background,MUII_BACKGROUND,FALSE,TAG_DONE),
					MUIA_Group_LayoutHook, &LayoutHook,
					End,
				TAG_MORE,msg->ops_AttrList)))
		return 0L;

	inst = INST_DATA(cl, obj);

	set(G_Forms,HTMLA_HTML_Data,inst); /* Make sure that the text class */
	set(G_Norm,HTMLA_HTML_Data,inst);  /*  knows about the text */


	inst->have_forms=0;
  
  inst->margin_width = 6;
  inst->margin_height = 4;
  inst->foreground = 1;
  inst->anchor_fg = 3;
  inst->visitedAnchor_fg = 1;
  inst->activeAnchor_bg = 0;
  inst->activeAnchor_fg = 2;
  inst->num_anchor_underlines = 1;
  inst->num_visitedAnchor_underlines = 1;
  inst->dashed_anchor_lines = FALSE;
  inst->dashed_visitedAnchor_lines = TRUE;
  inst->border_images = FALSE;
  inst->delay_images = TRUE;
  inst->is_index = FALSE;
  inst->percent_vert_space = 0;
  
  inst->raw_text = (char *)GetTagData(HTMLA_raw_text, (IPTR)NULL,
				      ((opSetP)msg)->ops_AttrList);
  inst->font = (TextFontP)GetTagData(HTMLA_font, (IPTR)NULL,
				     ((opSetP)msg)->ops_AttrList);
  inst->italic_font = (TextFontP)GetTagData(HTMLA_italic_font, (IPTR)NULL,
					    ((opSetP)msg)->ops_AttrList);
  inst->bold_font = (TextFontP)GetTagData(HTMLA_bold_font, (IPTR)NULL,
					  ((opSetP)msg)->ops_AttrList);
  inst->fixed_font = (TextFontP)GetTagData(HTMLA_fixed_font, (IPTR)NULL,
					   ((opSetP)msg)->ops_AttrList);
  inst->header1_font = (TextFontP)GetTagData(HTMLA_header1_font, (IPTR)NULL,
					     ((opSetP)msg)->ops_AttrList);
  inst->header2_font = (TextFontP)GetTagData(HTMLA_header2_font, (IPTR)NULL,
					     ((opSetP)msg)->ops_AttrList);
  inst->header3_font = (TextFontP)GetTagData(HTMLA_header3_font, (IPTR)NULL,
					     ((opSetP)msg)->ops_AttrList);
  inst->header4_font = (TextFontP)GetTagData(HTMLA_header4_font, (IPTR)NULL,
					     ((opSetP)msg)->ops_AttrList);
  inst->header5_font = (TextFontP)GetTagData(HTMLA_header5_font, (IPTR)NULL,
					     ((opSetP)msg)->ops_AttrList);
  inst->header6_font = (TextFontP)GetTagData(HTMLA_header6_font, (IPTR)NULL,
					     ((opSetP)msg)->ops_AttrList);
  inst->address_font = (TextFontP)GetTagData(HTMLA_address_font, (IPTR)NULL,
					     ((opSetP)msg)->ops_AttrList);
  inst->plain_font = (TextFontP)GetTagData(HTMLA_plain_font, (IPTR)NULL,
					   ((opSetP)msg)->ops_AttrList);
  inst->listing_font = (TextFontP)GetTagData(HTMLA_listing_font, (IPTR)NULL,
					     ((opSetP)msg)->ops_AttrList);
  inst->previously_visited_test =
   (visitTestProc)GetTagData(HTMLA_visit_func, (IPTR)NULL,
			     ((opSetP)msg)->ops_AttrList);
  inst->resolveImage =
   (resolveImageProc)GetTagData(HTMLA_image_func, (IPTR)NULL,
				((opSetP)msg)->ops_AttrList);

	inst->keyctrl = GetTagData(HTMLA_ScrollKeys, TRUE,
					     ((opSetP)msg)->ops_AttrList);
  
  inst->scroll_x = 0;
  inst->scroll_y = 0;
  inst->use_vbar = TRUE;
  inst->use_hbar = TRUE;
  
  inst->html_objects = HTMLParse(NULL, inst->raw_text);
  inst->html_header_objects = HTMLParse(NULL, inst->header_text);
  inst->html_footer_objects = HTMLParse(NULL, inst->footer_text);
  inst->formatted_elements = NULL;
  inst->my_visited_hrefs = NULL;
  inst->my_delayed_images = NULL;
	inst->widget_list = NULL;
  inst->form_list = NULL;
  inst->line_array = NULL;
  inst->line_count = 0;
  inst->max_pre_width = DocumentWidth(inst, inst->html_objects);
  inst->doc_width = inst->max_pre_width;
  inst->select_start = NULL;
  inst->select_end = NULL;
  inst->sel_start_pos = 0;
  inst->sel_end_pos = 0;
  inst->new_start = NULL;
  inst->new_end = NULL;
  inst->new_start_pos = 0;
  inst->new_end_pos = 0;
  inst->active_anchor = NULL;
  inst->cached_tracked_ele = NULL;
  inst->inlined_image_to_load = NULL;
  inst->active = TRUE;

  inst->reparsing=0;

  return (IPTR)obj;
}

static IPTR HTMLGadClDispose(Class *cl, Object *obj, Msg msg){
	return(DoSuperMethodA(cl,obj,msg));
}

#define C24CR_32(x) (((x&0xFF0000)<<8)|((x&0xFF0000))|((x&0xFF0000)>>8)|((x&0xFF0000)>>16))
#define C24CG_32(x) (((x&0xFF00)<<16)|((x&0xFF00)<<8)|((x&0xFF00))|((x&0xFF00)>>8))
#define C24CB_32(x) (((x&0xFF)<<24)|((x&0xFF)<<16)|((x&0xFF)<<8)|((x&0xFF)))

static IPTR HTMLGadClSetup(Class *cl, Object *obj, Msg msg){
	HTMLGadClData *inst=INST_DATA(cl,obj);
	long col;

	if (!DoSuperMethodA(cl,obj,msg))
		return FALSE;

  /* Allow input to gadget */
//  MUI_RequestIDCMP(obj, IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY);

	col = ObtainBestPen(_screen(obj)->ViewPort.ColorMap,C24CR_32(Rdata.u_anchor),C24CG_32(Rdata.u_anchor),C24CB_32(Rdata.u_anchor),
				OBP_Precision, PRECISION_IMAGE, TAG_DONE);
	if(col!=-1)
		inst->anchor_fg=col;
	else
		inst->anchor_fg=1;
	inst->anchor_fgc=col;

/*
I mean: FollowedLink, same with UnfollowedLink and
           ?ActiveLink
*/

	col = ObtainBestPen(_screen(obj)->ViewPort.ColorMap,C24CR_32(Rdata.f_anchor),C24CG_32(Rdata.f_anchor),C24CB_32(Rdata.f_anchor),
				OBP_Precision, PRECISION_IMAGE, TAG_DONE);
	if(col!=-1)
		inst->visitedAnchor_fg=col;
	else
		inst->visitedAnchor_fg=1;
	inst->visitedAnchor_fgc=col;

	col = ObtainBestPen(_screen(obj)->ViewPort.ColorMap,C24CR_32(Rdata.a_anchor),C24CG_32(Rdata.a_anchor),C24CB_32(Rdata.a_anchor),
				OBP_Precision, PRECISION_IMAGE, TAG_DONE);
	if(col!=-1)
		inst->activeAnchor_fg=col;
	else
		inst->activeAnchor_fg=2;
	inst->activeAnchor_fgc=col;

  return TRUE;
}

static IPTR HTMLGadClCleanup( Class *cl, Object *obj, Msg msg){
	HTMLGadClData *inst = INST_DATA(cl, obj);
//	MUI_RejectIDCMP(obj,IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY); /* and more */

//	TO_HTML_FlushImageCache();
	if(!inst->have_formsI){
		mo_flush_image_cache(&window);
//		kprintf("Flushing cache\n");
		}


	if(inst->anchor_fgc!=-1) ReleasePen(_screen(obj)->ViewPort.ColorMap, inst->anchor_fgc);
	if(inst->visitedAnchor_fgc!=-1) ReleasePen(_screen(obj)->ViewPort.ColorMap, inst->visitedAnchor_fgc);
	if(inst->activeAnchor_fgc!=-1) ReleasePen(_screen(obj)->ViewPort.ColorMap, inst->activeAnchor_fgc);

	return (DoSuperMethodA(cl,obj,msg));
}  


static IPTR HTMLGadClShow(Class *cl, Object *obj, Msg msg){
	HTMLGadClData *inst; 

	DoSuperMethodA(cl,obj,msg);

	inst = INST_DATA(cl, obj);

	inst->view_left = _mleft(obj);
	inst->view_top = _mtop(obj);
	inst->view_width = _mwidth(obj);
	inst->view_height = _mheight(obj);
	inst->rp=_rp(obj);
	inst->win=_window(obj);

	if(!inst->reparsing){
		inst->object_flags|=Object_Reparsing;
		if(!inst->have_forms)
			ReformatWindow(inst);
		SetAttrs(inst->SB_Vert,MUIA_Prop_Entries,inst->doc_height,
								MUIA_Prop_Visible,_mheight(obj),TAG_DONE);
		SetAttrs(inst->SB_Horiz,MUIA_Prop_Entries,inst->doc_width,
								MUIA_Prop_Visible,_mwidth(obj),TAG_DONE);
		inst->object_flags&=~Object_Reparsing;
		}
	return TRUE;
}

static IPTR HTMLGadClHide(Class *cl, Object *obj, Msg msg){
	HTMLGadClData *inst; 

	DoSuperMethodA(cl,obj,msg);

	inst = INST_DATA(cl, obj);

/*	inst->view_left		= -2242355;
	inst->view_top		= 1425;
	inst->view_width	= -23255;
	inst->view_height	= 3453664; */
	inst->rp			= NULL;
	inst->win			= NULL;

	return TRUE;
}

static IPTR HTMLGadClResetAmigaGadgets( Class *cl, Object *obj, Msg msg){

	HTMLGadClData *inst = INST_DATA(cl, obj);

	SetAttrs(inst->SB_Vert,MUIA_Prop_Entries,inst->doc_height,
							MUIA_Prop_Visible,_mheight(obj),TAG_DONE);

	SetAttrs(inst->SB_Horiz,MUIA_Prop_Entries,inst->doc_width,
							MUIA_Prop_Visible,_mwidth(obj),TAG_DONE);

	nnset(inst->SB_Vert,MUIA_Prop_First,inst->new_scroll_y);
	nnset(inst->SB_Horiz,MUIA_Prop_First,0);

	set(HTML_Gad,HTMLA_Top,inst->new_scroll_y);
	set(HTML_Gad,HTMLA_Left,0);

	if(!Rdata.isindex_to_form && inst->is_index)
		set(PG_Bottom, MUIA_Group_ActivePage, 2); /* Search gadgets */
	else
		set(PG_Bottom, MUIA_Group_ActivePage, 0); /* Text gadget */

//  set(LV_Source, MUIA_Floattext_Text, t);

	return TRUE;
}  

static IPTR HTMLGadClResetAmigaGadgetsNoScroll( Class *cl, Object *obj, Msg msg){

	HTMLGadClData *inst = INST_DATA(cl, obj);

	SetAttrs(inst->SB_Vert,MUIA_Prop_Entries,inst->doc_height,
							MUIA_Prop_Visible,_mheight(obj),TAG_DONE);

	SetAttrs(inst->SB_Horiz,MUIA_Prop_Entries,inst->doc_width,
							MUIA_Prop_Visible,_mwidth(obj),TAG_DONE);

	return TRUE;
}  

static IPTR HTMLGadClResetTopLeft( Class *cl, Object *obj, Msg msg){

	HTMLGadClData *inst = INST_DATA(cl, obj);

	nnset(inst->SB_Vert,MUIA_Prop_First,0);
	nnset(inst->SB_Horiz,MUIA_Prop_First,0);

	return TRUE;
}  

static IPTR HTMLGadClGet(Class *cl, Object *obj, Msg msg){

  HTMLGadClData *inst = INST_DATA(cl, obj);
  switch (((opGetP)msg)->opg_AttrID) {
  case HTMLA_title:
    *(((opGetP)msg)->opg_Storage) = (IPTR)(inst->title);
    break;
    
  case HTMLA_raw_text:
    *(((opGetP)msg)->opg_Storage) = (IPTR)(inst->raw_text);
    break;
    
  case HTMLA_image_load:
    *(((opGetP)msg)->opg_Storage) =
     (IPTR)(inst->inlined_image_to_load);
    break;
    
  case HTMLA_new_href:
    *(((opGetP)msg)->opg_Storage) =
     (IPTR)(inst->chosen_anchor);
    /*
       if ((IsDelayedHRef(inst, inst->chosen_anchor->anchorHRef)) ||
       (inst->chosen_anchor->pic_data->delayed == 2)) {
       *(((opGetP)msg)->opg_Storage) =
       (IPTR)(inst->chosen_anchor->edata);
       } else {
       *(((opGetP)msg)->opg_Storage) =
       (IPTR)(inst->chosen_anchor->anchorHRef);
       }
       */
    break;
  case HTMLA_post_text:
    *(((opGetP)msg)->opg_Storage) = (IPTR)inst->post_text;
    break;

  case HTMLA_post_href:
    *(((opGetP)msg)->opg_Storage) = (IPTR)inst->post_href;
    break;
   
  case HTMLA_get_href:
    *(((opGetP)msg)->opg_Storage) = (IPTR)inst->post_href;
    break;
   
  case HTMLA_doc_height:
    *(((opGetP)msg)->opg_Storage) = (IPTR)(inst->doc_height);
    break;

  case HTMLA_view_height:
    *(((opGetP)msg)->opg_Storage) = (IPTR)(inst->view_height);
    break;

  case HTMLA_doc_width:
    *(((opGetP)msg)->opg_Storage) = (IPTR)(inst->doc_width);
    break;
    
  case HTMLA_view_width:
	*(((opGetP)msg)->opg_Storage) = (IPTR)(inst->view_width);
	break;

  case HTMLA_anchor_fg:
    *(((opGetP)msg)->opg_Storage) = (IPTR)(inst->anchor_fg);
    break;
    
  case HTMLA_visited_fg:
    *(((opGetP)msg)->opg_Storage) = (IPTR)(inst->visitedAnchor_fg);
    break;
    
  case HTMLA_active_fg:
    *(((opGetP)msg)->opg_Storage) = (IPTR)(inst->activeAnchor_fg);
    break;
    
  case HTMLA_inst:
    *(((opGetP)msg)->opg_Storage) = (IPTR)(inst);
    break;
    
  case HTMLA_scroll_y:
    *(((opGetP)msg)->opg_Storage) = (IPTR)(inst->scroll_y);
    break;

  case HTMLA_Top:
    *(((opGetP)msg)->opg_Storage) = (IPTR)(inst->scroll_y);
    break;
  case HTMLA_Left:
    *(((opGetP)msg)->opg_Storage) = (IPTR)(inst->scroll_x);
    break;
  }
  
  return DoSuperMethodA(cl, obj, msg);
}

static IPTR HTMLGadClSet(Class *cl, Object *obj, Msg msg){
	HTMLGadClData *inst = INST_DATA(cl, obj);
	const struct TagItem *tstate;
	struct TagItem *ti;
	struct RastPort *rp;
  
	ti = ((opSetP)msg)->ops_AttrList;
	tstate = ti;
  
	while ((ti = NextTagItem(&tstate))) {
    switch (ti->ti_Tag) {
      
    case HTMLA_active:		inst->active = ti->ti_Data; break;
    case HTMLA_anchor_fg:	inst->anchor_fg = ti->ti_Data; break;
    case HTMLA_visited_fg:	inst->visitedAnchor_fg = ti->ti_Data; break;
    case HTMLA_active_fg:	inst->activeAnchor_fg = ti->ti_Data; break;
    case HTMLA_new_href:	break; /* Is this right? Do nothing? */
    case HTMLA_get_href:	inst->post_href=(char *)ti->ti_Data; break;
	case HTMLA_post_text:	inst->post_text=(char *)ti->ti_Data; break;
	case HTMLA_post_href:	inst->post_href=(char *)ti->ti_Data; break;
	case HTMLA_ScrollKeys:	inst->keyctrl=ti->ti_Data; break;
      
    case HTMLA_raw_text:
		rp = ObtainGIRPort(((opSetP)msg)->ops_GInfo);
		if (inst->rp == NULL) 
			inst->rp = rp;
		if (rp) {
			HTMLSetText(inst, (char *)ti->ti_Data, NULL, NULL, 0, NULL, NULL);
			ReleaseGIRPort(rp);
			}
		else {
			inst->raw_text = (char *)ti->ti_Data;
			inst->html_objects = NULL;
      		}
		break;
      
    case HTMLA_scroll_y:
/* Take care of the virtual group here */
/*		inst->new_scroll_value=ti->ti_Data;
		inst->new_scroll_dir=0; */


#if DEBUG0==1
		kprintf("HTML Main gad: Getting a new Y position: %ld\n",ti->ti_Data);
#endif
		nnset(inst->SB_Vert,MUIA_Prop_First,ti->ti_Data);
		if(inst->have_forms)
			set(G_Forms,HTMLA_Top,ti->ti_Data);
		else
			set(G_Norm,HTMLA_Top,ti->ti_Data);
		break;
      
    case HTMLA_scroll_x:
/* Take care of the virtual group here */
/*		inst->new_scroll_value=ti->ti_Data;
		inst->new_scroll_dir=1; */

#if DEBUG0==1
		kprintf("HTML Main gad: Getting a new X position: %ld\n",ti->ti_Data);
#endif
		nnset(inst->SB_Horiz,MUIA_Prop_First,ti->ti_Data);
		if(inst->have_forms)
			set(G_Forms,HTMLA_Left,ti->ti_Data);
		else
			set(G_Norm,HTMLA_Left,ti->ti_Data);

		break;
	case HTMLA_image_load: inst->inlined_image_to_load=(char *)ti->ti_Data; break;
	case HTMLA_SB_Vert:		inst->SB_Vert=(Object *)ti->ti_Data; break;
	case HTMLA_SB_Horiz:	inst->SB_Horiz=(Object *)ti->ti_Data; break;
	}
  }
  return DoSuperMethodA(cl, obj, msg);
}

static IPTR HandleInput(struct MUIP_HandleInput *msg)
{
	HTMLGadClData *inst = INST_DATA(HTMLGadClass, HTML_Gad);
	long data = 0;
	UWORD keycode;
	Object *obj=HTML_Gad;

	if (msg->imsg) {
		switch(msg->imsg->Class){

			case IDCMP_MOUSEBUTTONS:{
				switch (msg->imsg->Code){
				    case SELECTDOWN:
						if((msg->imsg->MouseX>=_mleft(obj)) && (msg->imsg->MouseX<=_mright(obj)) &&
						   (msg->imsg->MouseY>=_mtop(obj)) && (msg->imsg->MouseY<=_mbottom(obj))){
							new_button_press(inst,msg->imsg->MouseX-inst->view_left,msg->imsg->MouseY-inst->view_top);
							}
						break;
					case SELECTUP:		/* user let go of button */
						new_button_release(inst,obj,msg->imsg->MouseX-inst->view_left,msg->imsg->MouseY-inst->view_top);
						break;
					}
				}
				break;

			case IDCMP_MOUSEMOVE: {
				char *href, *old = NULL;
				struct ele_rec *eptr;
				int epos;
				if (Rdata.track_pointer_motion && inst->active) {
					if((msg->imsg->MouseX>=_mleft(obj)) && (msg->imsg->MouseX<=_mright(obj)) &&
					   (msg->imsg->MouseY>=_mtop(obj)) && (msg->imsg->MouseY<=_mbottom(obj))){

						eptr = LocateElement(inst, _window(obj)->MouseX - inst->view_left,
									_window(obj)->MouseY - inst->view_top, &epos);
						if (eptr && eptr->anchorHRef) {
							if (!strncmp(eptr->anchorHRef, "Delayed Image", 13))
								href = strdup(eptr->edata);	/* get the real URL name */
							else
								href = strdup(eptr->anchorHRef);
						href = mo_url_canonicalize_keep_anchor(href, window.cached_url);
						mo_convert_newlines_to_spaces(href);
						}
					else {
						href = strdup("");
						}
				}
				else href = strdup("");

				get(TX_Dest, MUIA_Text_Contents, &old);
				if ((!old) || (old && strcmp(old, href))) /* to minimize flashing */
					set(TX_Dest, MUIA_Text_Contents, href);
				free(href);
				}
			}
				break;

			case IDCMP_RAWKEY:
				if(inst->keyctrl){
					keycode=msg->imsg->Code;
					if(msg->imsg->Qualifier & IEQUALIFIER_SHIFT)
						keycode |= HIKEY_SHIFT;
					if(msg->imsg->Qualifier & IEQUALIFIER_ALT)
						keycode |= HIKEY_ALT;
					if(msg->imsg->Qualifier & IEQUALIFIER_CONTROL)
						keycode |= HIKEY_CTRL;
					switch (keycode){
						case 0x3E: /* Keypad 8 (up) */
						case CURSORUP:
						 	get(inst->SB_Vert, MUIA_Prop_First, &data);
							set(inst->SB_Vert, MUIA_Prop_First, data-10);
							break;

						case 0x1E: /* Keypad 2 (down) */
						case CURSORDOWN:
						 	get(inst->SB_Vert, MUIA_Prop_First, &data);
							set(inst->SB_Vert, MUIA_Prop_First, data+10);
							break;

						case 0x2D: /* Keypad 4 (left) */
						case CURSORLEFT:
						 	get(inst->SB_Horiz, MUIA_Prop_First, &data);
							set(inst->SB_Horiz, MUIA_Prop_First, data-10);
							break;

						case 0x2F: /* Keypad 6 (right) */
						case CURSORRIGHT:
						 	get(inst->SB_Horiz, MUIA_Prop_First, &data);
							set(inst->SB_Horiz, MUIA_Prop_First, data+10);
							break;

						case 0x1F: /* Keypad 3 (PgDn) */
						case (0x1E | HIKEY_SHIFT): /* Shift Keypad 2 (down) */
						case (CURSORDOWN | HIKEY_SHIFT):
						case 0x40: /* Space */
						 	get(inst->SB_Vert, MUIA_Prop_First, &data);
							set(inst->SB_Vert, MUIA_Prop_First, data+((_mheight(obj)*3)>>2));
							break;

						case 0x3F: /* Keypad 9 (PgUp) */
						case (0x3E | HIKEY_SHIFT): /* Shift Keypad 8 (up) */
						case (CURSORUP | HIKEY_SHIFT):
						case 0x41: /* Backspace */
						 	get(inst->SB_Vert, MUIA_Prop_First, &data);
							set(inst->SB_Vert, MUIA_Prop_First, data-((_mheight(obj)*3)>>2));
							break;

						case (CURSORLEFT | HIKEY_SHIFT):
						case (0x2D | HIKEY_SHIFT): /* Shift Keypad 4 (left) */
						 	get(inst->SB_Horiz, MUIA_Prop_First, &data);
							set(inst->SB_Horiz, MUIA_Prop_First, data-((_mwidth(obj)*3)>>2));
							break;

						case (0x2F | HIKEY_SHIFT): /* Shift Keypad 6 (right) */
						case (CURSORRIGHT | HIKEY_SHIFT):
						 	get(inst->SB_Horiz, MUIA_Prop_First, &data);
							set(inst->SB_Horiz, MUIA_Prop_First, data+((_mwidth(obj)*3)>>2));
							break;

						case 0x3D: /* Keypad 7 (Home) */
						case (0x3E | HIKEY_CTRL): /* Ctrl Keypad 8 (up) */
						case (CURSORUP | HIKEY_CTRL):
							set(inst->SB_Vert, MUIA_Prop_First,0); /* Beg of file */
							break;

						case 0x1D: /* Keypad 1 (End) */
						case (0x1E | HIKEY_CTRL): /* Ctrl Keypad 2 (down) */
						case (CURSORDOWN | HIKEY_CTRL):
							set(inst->SB_Vert, MUIA_Prop_First,inst->doc_height-_mheight(obj)); /* End of file */
							break;

						case (0x2D | HIKEY_CTRL): /* Ctrl Keypad 4 (left) */
						case (CURSORLEFT | HIKEY_CTRL):
							set(inst->SB_Horiz, MUIA_Prop_First,0); /* Beg of file */
							break;

						case (0x2F | HIKEY_CTRL): /* Ctrl Keypad 6 (right) */
						case (CURSORRIGHT | HIKEY_CTRL):
							set(inst->SB_Horiz, MUIA_Prop_First,inst->doc_width-_mwidth(obj)); /* End of file */
							break;
						}
					}
				break;
			}
      }
	return 0;
}


#if 0
/*------------------------------------------------------------------------
  Draw the HTML Gadget
  ------------------------------------------------------------------------*/
static IPTR HTMLGadClDraw(Class *cl, Object *obj, struct MUIP_Draw *msg){
	HTMLGadClData *HTML_Data = INST_DATA(cl, obj);

	DoSuperMethodA(cl,obj,(Msg)msg);

//	if(!(msg->flags & MADF_DRAWOBJECT) && !(msg->flags & MADF_DRAWUPDATE))
//		return 0L;

//	if(msg->flags & MADF_DRAWUPDATE) /* scroll to position */
//		ScrollToPos(HTML_Data, HTML_Data->new_scroll_dir, HTML_Data->new_scroll_value);

//	if(msg->flags & MADF_DRAWOBJECT) /* total redraw */
//		{
//		/* This is necessary in case the HTMLSetText was called before the gadget
//		   was ready, i.e. after a window resize.  This is MUI's fault (still!) */
//  
//		if (HTML_Data->html_objects == NULL) {
//    NotifyChange(cl, (Object *)g, (struct gpInput *)msg, HTML_mui_resize, (IPTR)NULL);
//			ResetAmigaGadgets(); /* Hope this is correct */
//			HTMLSetText(HTML_Data, HTML_Data->raw_text, NULL, NULL, 0, NULL, NULL);
// 			}

/*  ReformatWindow(HTML_Data); */ /* This is done in show */
//	ViewClearAndRefresh(HTML_Data);
//		}
//	DoSuperMethodA(cl,obj,(Msg)msg);

	return 0;
}
#endif

SAVEDS IPTR HTMLGadClPlaceObject(struct IClass *cl,Object *obj,struct MUIP_PlaceObject *msg)
{
	HTMLGadClData *inst = INST_DATA(cl, obj);

	if(MUIMasterBase->lib_Version<11){
		if(inst->have_forms){
//			kprintf("Custom layout\n");
			inst->view_left = _mleft(HTML_Gad);
			inst->view_top = _mtop(HTML_Gad);
			inst->view_width = _mwidth(HTML_Gad);
			inst->view_height = _mheight(HTML_Gad);
			inst->rp=_rp(HTML_Gad);
			inst->win=_window(HTML_Gad);

			inst->object_flags|=Object_Reparsing;
			ReformatWindow(inst);
			inst->object_flags&=~Object_Reparsing;
			}
		return DoSuperMethodA(cl,obj,(Msg)msg);
		}
	else
		return DoSuperMethodA(cl,obj,(Msg)msg);
}

/* WORKING ON THIS */
/* TEMP COMMENT OUT */
static IPTR HTMLGadClSetText(Class *cl, Object *obj, Msg msg)
{
 return 0;
}
#if FALSE
static IPTR   HTMLGadClSetText(Class *cl, Object *obj, Msg msg)
{
  HTMLGadClData *inst = INST_DATA(cl, o);
  struct TagItem *tstate, *ti;
  /* Depends on header_text, footer_text, element_id, target_anchor
     being set first
     inst->widget_list = wptr; DEPEND 
     if (text != NULL) {
     if (*text == '\0'){
     text = NULL;
     }
     inst->raw_text = text;
     }
     
     if (header_text != NULL)    {
     if (*header_text == '\0')
     {
     header_text = NULL;
     }
     inst->header_text = header_text;
     }
     if (footer_text != NULL) {
     if (*footer_text == '\0')
     {
     footer_text = NULL;
     }
     inst->footer_text = footer_text;
     }
     */
  DoSuperMethodA(cl, o, msg);  /* probably not needed */
  /* Free up the old visited href list.  */
  FreeHRefs(inst->my_visited_hrefs);
  inst->my_visited_hrefs = NULL;
  
      
  /* Free up the old visited delayed images list.  */
    FreeDelayedImages(inst->my_delayed_images);
  inst->my_delayed_images = NULL;
  
  /*   * Hide any old widgets */
  //	HideWidgets(inst);
  /*   inst->widget_list = wptr; DEPEND */
  inst->form_list = NULL;
  
  if (inst->raw_text != NULL) {
    inst->raw_text = text;
    /* Free any old colors and pixmaps    */
    // FreeColors(XtDisplay(inst),DefaultColormapOfScreen(XtScreen(inst)));
    // FreeImages(inst);
    
    /* Parse the raw text with the HTML parser */
    inst->html_objects = HTMLParse(inst->html_objects, inst->raw_text);
    CallLinkCallbacks(inst); /* What do this do? [looks risky]*/
  }
  if (inst->header_text != NULL)    {
    /* Parse the header text with the HTML parser */
    inst->html_header_objects =
      HTMLParse(inst->html_header_objects, inst->header_text);
  }
  if (inst->footer_text != NULL){
    /** Parse the footer text with the HTML parser     */
    inst->html_footer_objects =
      HTMLParse(inst->html_footer_objects, inst->footer_text);
  }
  
  /* Reformat the new text */
  inst->max_pre_width = DocumentWidth(inst, inst->html_objects);
  ReformatWindow(inst); /* Does this try to do a display?????*/
  
HERE
  /*
   * If a target anchor is passed, override the element id
   * with the id of that anchor.
   */
  if (target_anchor != NULL)
    {
      int id;
      
      id = HTMLAnchorToId(w, target_anchor);
      if (id != 0)
	{
	  element_id = id;
	}
    }
  
  /*
   * Position text at id specified, or at top if no position
   * specified.
   * Find the element corrsponding to the id passed in.
   */
  eptr = NULL;
  if (element_id != 0)
    {
      start = inst->formatted_elements;
      while (start != NULL)
	{
	  if (start->ele_id == element_id)
	    {
	      eptr = start;
	      break;
	    }
	  start = start->next;
	}
    }
  if (eptr == NULL)
    {
      newy = 0;
    }
  else
    {
      newy = eptr->y - 2;
    }
  if (newy < 0)
    {
      newy = 0;
    }
  if (newy > (inst->doc_height - (int)inst->view_height))
    {
      newy = inst->doc_height - (int)inst->view_height;
    }
  if (newy < 0)
    {
      newy = 0;
    }
  inst->scroll_x = 0;
  inst->scroll_y = newy;
  //	ConfigScrollBars(inst);
  //	ScrollWidgets(inst);
  
  /*
   * Display the new text
   */
  ViewClearAndRefresh(inst);
  
  /*
   * Clear any previous selection
   */
  inst->select_start = NULL;
  inst->select_end = NULL;
  inst->sel_start_pos = 0;
  inst->sel_end_pos = 0;
  inst->new_start = NULL;
  inst->new_end = NULL;
  inst->new_start_pos = 0;
  inst->new_end_pos = 0;
  inst->active_anchor = NULL;
  
  inst->cached_tracked_ele = NULL;
}

#endif

/********************************************************
  The main gadget handler routine
********************************************************/
DISPATCHER(HTMLGadClDispatch)
{
/*  kprintf("Method %08lx issued\n",msg->MethodID); */

  switch (msg->MethodID) { /* approx calling order */
  case OM_NEW          : return(HTMLGadClNew(cl,obj,(APTR)msg));
  case OM_DISPOSE      : return(HTMLGadClDispose(cl,obj,(APTR)msg));
  case MUIM_AskMinMax  : return(HTMLGadClAskMinMax(cl,obj,(APTR)msg));
  case MUIM_Setup      : return(HTMLGadClSetup(cl,obj,(APTR)msg));
  case MUIM_Show       : return(HTMLGadClShow(cl,obj,(APTR)msg));
  case MUIM_Hide       : return(HTMLGadClHide(cl,obj,(APTR)msg));
//  case MUIM_Draw       : return(HTMLGadClDraw(cl,obj,(APTR)msg));
//  case MUIM_HandleInput: return(HTMLGadClHandleInput(cl,obj,(APTR)msg));
  case OM_GET          : return(HTMLGadClGet(cl,obj,(APTR)msg));
  case OM_SET          : return(HTMLGadClSet(cl,obj,(APTR)msg));
  case MUIM_Cleanup    : return(HTMLGadClCleanup(cl,obj,(APTR)msg)); 

  /* What follows is a set of methods for doing things that used to
     be done illegally by messing with the HTMLGadget from outside it.
     They are called by cover functions in TO_HTML.c (and maybe 
     other places too, since this is legal). Note that we will
     usually have to use "set" immdiately before calling these
     methods, to prepare the structures they use. */ 

  case HTMLM_SetText   : return(HTMLGadClSetText(cl,obj,(APTR)msg));
  case HTMLM_ResetAmigaGadgets : return(HTMLGadClResetAmigaGadgets(cl,obj,(APTR)msg));
  case HTMLM_ResetAmigaGadgetsNoScroll : return(HTMLGadClResetAmigaGadgetsNoScroll(cl,obj,(APTR)msg));
  case HTMLM_ResetTopLeft : return(HTMLGadClResetTopLeft(cl,obj,(APTR)msg));
 
/* Methods to be implemented:
	HTMLM_LoadInlinedImages:
	HTMLM_LoadInlinedImage:
    HTMLM_ClearImageCache:
    HTMLM_GotoAnchor:
*/


  }

	if(mui_version<11)
		if(msg->MethodID==MUIM_PlaceObject)
			return(HTMLGadClPlaceObject(cl,obj,(APTR)msg));


  return(DoSuperMethodA(cl,obj,msg));
}



void new_button_press(HTMLGadClData *HTML_Data,long x,long y)
{
	struct ele_rec *eptr;
	int epos;

	if (HTML_Data->active == FALSE) return;

	eptr = LocateElement(HTML_Data, x, y, &epos);

	if (eptr != NULL) {
		if (eptr->anchorHRef != NULL) {
			HTML_Data->active_anchor = eptr;
			HTML_Data->press_x = x;
			HTML_Data->press_y = y;
			SetAnchor(HTML_Data);
			}
		else {
			HTML_Data->new_start = NULL;
			HTML_Data->new_end = NULL;
			HTML_Data->new_start_pos = 0;
			HTML_Data->new_end_pos = 0;
			HTML_Data->press_x = x;
			HTML_Data->press_y = y;
			}
		}
	else {
		HTML_Data->new_start = NULL;
		HTML_Data->new_end = NULL;
		HTML_Data->new_start_pos = 0;
		HTML_Data->new_end_pos = 0;
		HTML_Data->press_x = x;
		HTML_Data->press_y = y;
		}
}   

/*
 * If we just released button one or two, and we are on an object,
 * and we have an active anchor, and we are on the active anchor,
 * and if we havn't waited too long.  Activate that anchor.
 */
void new_button_release(HTMLGadClData *HTML_Data,Object *obj,long x,long y)
{
	struct ele_rec *eptr;
	int epos;

	eptr = LocateElement(HTML_Data, x, y, &epos);

	if ((eptr != NULL)&&(eptr == HTML_Data->active_anchor)){
		if (eptr->anchorHRef != NULL){
			if ((IsDelayedHRef(HTML_Data, eptr->anchorHRef))&&
				(HTML_Data->resolveImage != NULL)){	/* Resolve pic */
				eptr->pic_data = (*(resolveImageProc)
					(HTML_Data->resolveImage))(HTML_Data, eptr->edata,0);
					/* MDF KLUDGE: If the image isn't in the cache (i.e. if the
					   resolve proc returns null) we CANNOT load it over the net.
					   We must tell the GUI that we need it to load the image, and
					   then refresh us.  What a hassle. */

				if (eptr->pic_data == NULL) {
					HTML_Data->inlined_image_to_load = eptr->edata;
					set(obj,HTMLA_image_load,(IPTR)(eptr->edata));
					set(G_Norm,HTMLA_image_load,(IPTR)(eptr->edata));
/*					if(eptr->pic_data)
						eptr->pic_data->delayed = 0;
					else
						kprintf("Image Not loaded\n"); */
					}
	    		}
			else if ((eptr->pic_data != NULL)&&
				(eptr->pic_data->delayed)&&
				(eptr->anchorHRef != NULL)&&
				(!IsDelayedHRef(HTML_Data, eptr->anchorHRef))&&
				((y + HTML_Data->scroll_y - eptr->y) > AnchoredHeight(HTML_Data))) {
					eptr->pic_data = (*(resolveImageProc)
						(HTML_Data->resolveImage))(HTML_Data, eptr->edata,0);
						/* MDF KLUDGE: If the image isn't in the cache (i.e. if the
						   resolve proc returns null) we CANNOT load it over the net.
						   We must tell the GUI that we need it to load the image, and
						   then refresh us.  What a hassle. */

					if (eptr->pic_data == NULL) {
						HTML_Data->inlined_image_to_load = eptr->edata;
						set(obj,HTMLA_image_load,(IPTR)(eptr->edata));
						set(G_Norm,HTMLA_image_load,(IPTR)(eptr->edata));
/*						if(eptr->pic_data)
							eptr->pic_data->delayed = 0;
						else
							kprintf("Image Not loaded\n"); */
						}
				}
			    else if ((eptr->pic_data != NULL)&&
					(eptr->pic_data->ismap)&&
					(eptr->anchorHRef != NULL)&&
					(IsIsMapForm(HTML_Data, eptr->anchorHRef))) {
						ImageSubmitForm(eptr->pic_data->fptr,
						eptr->pic_data->text, x + HTML_Data->scroll_x - eptr->x,
							y + HTML_Data->scroll_y -	eptr->y);
			    	}

			else {
	      /* The following is a hack to send the
	       * selection location along with the HRef
	       * for images.  This allows you to
	       * point at a location on a map and have
	       * the server send you the related document.
	       * Tony Sanders, April 1993 <sanders@bsdi.com>
	       */
				char *buf = eptr->anchorHRef;
				if (eptr->type == E_IMAGE && eptr->pic_data
					&& eptr->pic_data->ismap) {
					buf = (char *)
					malloc(strlen(eptr->anchorHRef) + 256);
					sprintf(buf, "%s?%d,%d",
					eptr->anchorHRef,
					(int)(x + HTML_Data->scroll_x - eptr->x),
					(int)(y + HTML_Data->scroll_y - eptr->y));
					}
				HTML_Data->chosen_anchor = buf;
				set(obj,HTMLA_new_href,(IPTR)(buf));
				set(G_Norm,HTMLA_new_href,(IPTR)(buf));
	    		}
			}
		}
	UnsetAnchor(HTML_Data);
}


struct Region *installClipRegion (struct Window *w, struct Layer *l, struct Region *r)
{
    BOOL refresh = FALSE;
    struct Region *or;

    if (w->Flags & WFLG_WINDOWREFRESH)
    {
        EndRefresh (w, FALSE);
        refresh = TRUE;
    }
    or = InstallClipRegion (l, r);
    if (refresh)
        BeginRefresh (w);
    return (or);
}


/*********************************************************************
  NAME: LoadInlinedImage(char *);

  DESC: Load a single inlined image over the network into the image
        cache, and refresh the HTML gadget to display the image.

  INPUTS: char *img -- the URL of the image to load.

  RETURNS: 1 if all went well, 0 if there was a problem.

  CALLS: ImageLoadAndCache() in img.c, to do the main work.
**********************************************************************/  
static long HTMLid=-1;
static long HTMLidscroll=-1;
long HTMLFormImages=0;

int LoadInlinedImage(char *img)
{
  HTMLGadClData *inst;
  void *ptr;
  char *url = mo_url_canonicalize(img, cached_url);
  int ret=0;
  long id;

  if (strncmp(img, "internal-", 9) == 0) return 1;

  if (lib_version < 39) {	/* WB 3.x needed! */
    return 0;
  }

	inst=INST_DATA(HTMLGadClass,HTML_Gad);

  ptr = mo_fetch_cached_image_data(url);
  if (!ptr) {		/* we don't have it in cache */
    inst->active = FALSE;
	Rdata.track_pointer_motion=FALSE;
    ret = ImageLoadAndCache(inst, url);
	Rdata.track_pointer_motion=TRUE;

	if(!inst->have_forms){
		DoMethod(App,MUIM_Application_InputBuffered);

		if((HTMLid==-1) || (inst->scroll_y!=HTMLidscroll)){
			HTMLid=id=HTMLPositionToId(inst,inst->scroll_y+2,0);
			}
		else
			id=HTMLid;

		ReformatWindow(inst);
		DoMethod(HTML_Gad,HTMLM_ResetAmigaGadgetsNoScroll);
		MUI_Redraw(HTML_Gad,MADF_DRAWOBJECT);
		HTMLGotoId(inst,id);
		HTMLidscroll=inst->scroll_y;
		}
	else
		if(ret)
			HTMLFormImages++;

	   	inst->active = TRUE;
	}

	return ret;
}



/* These have still to be looked at  (were in main.c)*/

/*********************************************************************
  NAME: LoadInlinedImages();

  DESC: Load from the network all the inlined images needed by the
        current HTML document into the image cache.

  INPUTS: none.

  CALLS: LoadInlinedImage() for each needed image.
**********************************************************************/  
extern long image_interrupt;
int LoadInlinedImages()
{
	HTMLGadClData *inst=INST_DATA(HTMLGadClass,HTML_Gad);
	char **hrefs;
	int num, i;
  
	if (lib_version < 39) {	/* WB 3.x needed! */
    	return 0;
		}

	HTMLFormImages=0;

	if(!inst->have_forms){
		HTMLid=HTMLPositionToId(inst,inst->scroll_y+2,0);
		HTMLidscroll=inst->scroll_y;
		}

	hrefs = HTMLGetImageSrcs (inst, &(num));
	image_interrupt=0;

	if (num) {
		for (i=0; i<num; i++) {
			if(!image_interrupt)
				LoadInlinedImage(hrefs[i]);
			free (hrefs[i]);
			}
		free (hrefs);
		}

	if(inst->have_forms && HTMLFormImages)
		set(WI_ImagesReady,MUIA_Window_Open,TRUE);

	HTMLid=-1;
	return 0;
}

/*********************************************************************
  NAME: NotifyChange()

  PURPOSE: Send notification event to Intuition that an attribute of
           the Boopsi gadget has changed.
*********************************************************************/
void NotifyChange(Class *cl, Object *o, struct gpInput *gpi, Tag tag,
		  IPTR data){
  struct TagItem tt[3];

  tt[0].ti_Tag = tag;
  tt[0].ti_Data = data;

  tt[1].ti_Tag = GA_ID;
  tt[1].ti_Data = ((struct Gadget *)o)->GadgetID;

  tt[2].ti_Tag = TAG_DONE;

  DoSuperMethod(cl, o, OM_NOTIFY, tt, gpi->gpi_GInfo, 0);
}

/*********************************************************************
  NAME: ResetAmigaGadgets();

  DESC: After any window document is changed, this should be called to
        update the scrollbar thumb sizes and locations, and to enable
	or disable the ISINDEX search gadgets.        

  INPUTS: none.
**********************************************************************/  
void ResetAmigaGadgets(void)
{
	DoMethod(HTML_Gad,HTMLM_ResetAmigaGadgets);
}

/**********OLD JUNK **********/
/***********************************
  Memory to store information about Boopsi HTML Gadget Objects
 ***********************************/
#if FALSE
typedef struct plitem {
  struct plitem *prev;
  struct plitem *next;
  HTMLPart *part;
} HTMLPartItem;
typedef HTMLPartItem  *HTMLPartList;
static HTMLPartList globallist = (HTMLPartList)NULL;

HTMLPart ThePart;

static HTMLPart *RememberNEWHTMLAmigaData(void){
  HTMLPartItem *NewItem;
  HTMLPart     *NewPart;
  
  if (NULL == (NewPart = (HTMLPart *)malloc(sizeof(HTMLPart)))) return NULL;
  if (NULL == (NewItem = (HTMLPartItem *)malloc(sizeof(HTMLPartItem)))) {
    free(NewPart); return NULL;
  }
  NewItem->prev = (HTMLPartItem *)NULL;
  NewItem->next = (HTMLPartItem *)NULL;
  NewItem->part = NewPart;

  if ((HTMLPartList)NULL != globallist){
    NewItem->next = globallist;
    globallist->prev = NewItem;
  }
  globallist = NewItem;
  return NewPart;
}

/* public */
void ForgetHTMLAmigaData(HTMLPart * item){
  HTMLPartItem *ItemPtr;
  for (ItemPtr = globallist; (ItemPtr != NULL); ItemPtr= ItemPtr->next ) {
    if (ItemPtr->part == item){
      /* Remove if from the list */
      if (ItemPtr->prev == NULL){
	globallist = ItemPtr->next;
	if (globallist != NULL){
	  globallist->prev = NULL;
	}
      } else {
	ItemPtr->prev->next = ItemPtr->next;
	if (ItemPtr->next != NULL){
	  ItemPtr->next->prev = ItemPtr->prev;
	}
      }
      /* It's out of the globallist. Destroy it */
      if (ItemPtr->part != NULL) free(ItemPtr->part);
      free(ItemPtr);
      break;
    }
  }
}
/* public */
void ForgetALLHTMLAmigaData  (void){
/* Call before final exit */
  while (globallist != NULL){
    ForgetHTMLAmigaData(globallist->part);
  }
}

static void CheckShadowing (HTMLGadClData *inst){
/*  the htmlshadow is there so that we can detect when MUI has
    remade the widget, using remember to copy over html
    then we can delete the memory that was allocated when
    the replacement gadget was created (i.e. that pointed to
    by the shadow, and make the shadow == the original again */
/*  if (inst == NULL) return;
  if (inst == NULL) return;
  if (instshadow != NULL) {
    ForgetHTMLAmigaData(instshadow);
  }
  instshadow = inst;*/
  return;  
}
#endif

/* This was in HTML_img.c */

/*********************************************************************
  NAME: ImageLoadAndCache(HTMLPart *hw, char *src)

  DESC: Load a single inlined image over the network.  Use DataTypes
        to convert it to an Amiga BitMap.  Save the image in the
	cache.

  INPUTS: HTMLPart *hw -- pointer to the HTML gadget.
          char *img -- the _canonicalized_ URL of the image to load.

  RETURNS: 1 if all went well, 0 if there was a problem.

  CALLS: mo_cache_data()

  COMMENTS: We need to make our own copy of the returned BitMap so
            that we can dispose of the DataTypes object and remove
            the temporary file in T:
**********************************************************************/  

/* The URL should already be canonicalized.  We should know that the image
   isn't already in the cache.  */

extern int lib_version;

#if 0
/* This routine doesn't use transparent gifs */
int ImageLoadAndCache(HTMLGadClData *HTML_Data, char *src)
{
  APTR dtobject;
  struct BitMap *bm1 = NULL, *bm2;
  struct BitMapHeader *bmh = NULL;
  struct gpLayout mygpLayout;
  ImageInfo *img_data;
  char *fnam;
  int rc;

  if (!src) {
    return 0;
  }

  if (lib_version < 39) {	/* WB 3.x needed! */
    return 0;
  }

  fnam = mo_tmpnam();
  rc = mo_pull_er_over_virgin (src, fnam);

  if (!rc) {
    free (fnam);
    return 0;
  }

	img_data = malloc(sizeof(ImageInfo));
	img_data->mask_bitmap=NULL;
 
	if (dtobject = NewDTObject(fnam, DTA_GroupID,GID_PICTURE, PDTA_Remap, TRUE, TAG_DONE)){
		IPTR *pentable;
		int numpens;
		struct BitMap *OrgBitMap;

		GetDTAttrs(dtobject, PDTA_BitMapHeader, &bmh, TAG_DONE);

		SetDTAttrs(dtobject, NULL,NULL, PDTA_Screen, HTML_Data->win->WScreen,
			PDTA_FreeSourceBitMap,TRUE,TAG_DONE);

//		DoDTMethod(dtobject,NULL, NULL, DTM_PROCLAYOUT, NULL,1);
      	DoMethod(dtobject,DTM_PROCLAYOUT,NULL,1L); /* Doesn't lock up the system while remapping */

		GetDTAttrs(dtobject, PDTA_BitMap, &bm1,
			PDTA_ColorTable, &pentable, PDTA_Allocated, &numpens, TAG_DONE);

		img_data->width = bmh->bmh_Width;
		img_data->height = bmh->bmh_Height;
		img_data->image_data = (char *)1;
		img_data->image = bm1;
		img_data->dt = dtobject;
		img_data->colors = pentable;
		img_data->fname = fnam;

		mo_cache_data (src, (void *)img_data, 0);
		}
 else {
      FILE *fp;
      unsigned char *data;
      int width, height, colrs, bg;

      fp = fopen(fnam, "r");

      data = ReadXbmBitmap(fp, fnam, &width, &height, &colrs);
      fclose(fp);
      if (data) {
	bm1 = CreateBitMapFromXBMData(data, width, height,
 				      WIDG_DEPTH);
	img_data->width = width;
	img_data->height = height;
	img_data->image_data = (char *)1;
	img_data->image = bm1;
	img_data->dt = NULL;
	img_data->colors = NULL;
	img_data->fname = NULL;
	img_data->mask_bitmap=NULL;
      
	mo_cache_data (src, (void *)img_data, 0);

	unlink(fnam);
	free (fnam);

      } else {

	unlink(fnam);
	free (fnam);
	free(img_data);
	return 0;		/* datatype conversion failed! */
      }
    }      

  return 1;
}

#endif
/* This routine handles transparent gifs. However, it seems like it
doesn't work under cybergfx */
#if 0
int ImageLoadAndCache(HTMLGadClData *HTML_Data, char *src)
{
  APTR dtobject;
  struct BitMap *bm1 = NULL, *bm2;
  struct BitMapHeader *bmh = NULL;
  struct gpLayout mygpLayout;
  ImageInfo *img_data;
  char *fnam;
  int rc;

  if (!src) {
    return 0;
  }

  if (lib_version < 39) {	/* WB 3.x needed! */
    return 0;
  }

  fnam = mo_tmpnam();
  rc = mo_pull_er_over_virgin (src, fnam);

  if (!rc) {
    free (fnam);
    return 0;
  }

	img_data = malloc(sizeof(ImageInfo));
	img_data->mask_bitmap=NULL;
 
	if (dtobject = NewDTObject(fnam, DTA_GroupID,GID_PICTURE, PDTA_Remap, FALSE, TAG_DONE)){
		IPTR *pentable;
		int numpens;
		struct BitMap *OrgBitMap;

		GetDTAttrs(dtobject, PDTA_BitMapHeader, &bmh, TAG_DONE);

		/* Use a faked transparent color */
//		bmh->bmh_Masking=mskHasTransparentColor;
//		bmh->bmh_Transparent=0; /* Color 0 is transparent */
		if(bmh->bmh_Masking==mskHasTransparentColor){
			struct RastPort *tmpRp1,*tmpRp2;
			struct BitMap *b;
			long x,y,c=bmh->bmh_Transparent;

#if DEBUG1==1
			kprintf("Transparent picture\n");
#endif

//			DoDTMethod(dtobject,NULL, NULL, DTM_PROCLAYOUT, NULL,1);
	      	DoMethod(dtobject,DTM_PROCLAYOUT,NULL,1L); /* Doesn't lock up the system while remapping */

			GetDTAttrs(dtobject, PDTA_BitMap, &OrgBitMap, TAG_DONE);

//			b = AllocBitMap(bmh->bmh_Width, bmh->bmh_Height,1, BMF_CLEAR, OrgBitMap);

			tmpRp1=calloc(1,sizeof(struct RastPort));
//			tmpRp2=calloc(1,sizeof(struct RastPort));


			InitRastPort(tmpRp1);
			InitRastPort(tmpRp2);


			tmpRp1->BitMap=OrgBitMap;
			tmpRp2->BitMap=b;

			SetABPenDrMd(tmpRp2,1,1,JAM1);

/*			for(x=0;x<bmh->bmh_Width;x++)
				for(y=0;y<bmh->bmh_Height;y++)
					if(ReadPixel(tmpRp1,x,y)!=c)
						WritePixel(tmpRp2,x,y); */
			if(tmpRp1) free(tmpRp1);
			if(tmpRp2) free(tmpRp2);

//			kprintf("Bmap0 %lx,",b->Planes[0]);
//			kprintf("Bmap1 %lx,",b->Planes[1]);
//			kprintf("Bmap2 %lx,",b->Planes[2]);
//			kprintf("Bmap3 %lx.\n",b->Planes[3]);

			img_data->mask_bitmap=b;
			}
#if DEBUG1==1
		else
		kprintf("Nontransparent picture\n");
#endif

		SetDTAttrs(dtobject, NULL,NULL, PDTA_Screen, HTML_Data->win->WScreen,
			PDTA_FreeSourceBitMap,TRUE,PDTA_Remap,TRUE, TAG_DONE);

//		DoDTMethod(dtobject,NULL, NULL, DTM_PROCLAYOUT, NULL,1);
      	DoMethod(dtobject,DTM_PROCLAYOUT,NULL,1L); /* Doesn't lock up the system while remapping */

		GetDTAttrs(dtobject, PDTA_BitMap, &bm1,
			PDTA_ColorTable, &pentable, PDTA_Allocated, &numpens, TAG_DONE);

		img_data->width = bmh->bmh_Width;
		img_data->height = bmh->bmh_Height;
		img_data->image_data = (char *)1;
		img_data->image = bm1;
		img_data->dt = dtobject;
		img_data->colors = pentable;
		img_data->fname = fnam;

		mo_cache_data (src, (void *)img_data, 0);
		}
 else {
      FILE *fp;
      unsigned char *data;
      int width, height, colrs, bg;

      fp = fopen(fnam, "r");

      data = ReadXbmBitmap(fp, fnam, &width, &height, &colrs);
      fclose(fp);
      if (data) {
	bm1 = CreateBitMapFromXBMData(data, width, height,
 				      WIDG_DEPTH);
	img_data->width = width;
	img_data->height = height;
	img_data->image_data = (char *)1;
	img_data->image = bm1;
	img_data->dt = NULL;
	img_data->colors = NULL;
	img_data->fname = NULL;
	img_data->mask_bitmap=NULL;
      
	mo_cache_data (src, (void *)img_data, 0);

	unlink(fnam);
	free (fnam);

      } else {

	unlink(fnam);
	free (fnam);
	free(img_data);
	return 0;		/* datatype conversion failed! */
      }
    }      

  return 1;
}
#endif

int ImageLoadAndCache(HTMLGadClData *HTML_Data, char *src)
{
	APTR dtobject;
	struct BitMap *bm1 = NULL;
	struct BitMapHeader *bmh = NULL;
	ImageInfo *img_data;
	char *fnam;
	int rc;
	struct RastPort *tmpRp1,*tmpRp2 = NULL;
	struct BitMap *b;

  if (!src) {
    return 0;
  }

  if (lib_version < 39) {	/* WB 3.x needed! */
    return 0;
  }

  fnam = mo_tmpnam();
  rc = mo_pull_er_over_virgin (src, fnam);

  if (!rc) {
    free (fnam);
    return 0;
  }

	img_data = malloc(sizeof(ImageInfo));
	img_data->mask_bitmap=NULL;
 
	if ((dtobject = NewDTObject(fnam, DTA_GroupID,GID_PICTURE, PDTA_Remap, FALSE, TAG_DONE))){
		ULONG *pentable;
		int numpens;
		struct BitMap *OrgBitMap;

		GetDTAttrs(dtobject, PDTA_BitMapHeader, &bmh, TAG_DONE);

		if(bmh->bmh_Masking==mskHasTransparentColor){
			long x,y,c=bmh->bmh_Transparent;

#if DEBUG1==1
			kprintf("Transparent picture\n");
#endif

	      	DoMethod(dtobject,DTM_PROCLAYOUT,NULL,1L); /* Doesn't lock up the system while remapping */
			GetDTAttrs(dtobject, PDTA_BitMap, &OrgBitMap, TAG_DONE);

			b = AllocBitMap(bmh->bmh_Width, bmh->bmh_Height,1, BMF_CLEAR, OrgBitMap);

			tmpRp1=calloc(1,sizeof(struct RastPort));
			tmpRp2=calloc(1,sizeof(struct RastPort));

			InitRastPort(tmpRp1);
			InitRastPort(tmpRp2);

			tmpRp1->BitMap=OrgBitMap;
			tmpRp2->BitMap=b;

			SetABPenDrMd(tmpRp2,1,1,JAM1);

			for(x=0;x<bmh->bmh_Width;x++)
				for(y=0;y<bmh->bmh_Height;y++)
					if(ReadPixel(tmpRp1,x,y)!=c)
						WritePixel(tmpRp2,x,y);

			}

		SetDTAttrs(dtobject, NULL,NULL, PDTA_Screen, HTML_Data->win->WScreen,
			PDTA_FreeSourceBitMap,TRUE,PDTA_Remap,TRUE, TAG_DONE);

//		DoDTMethod(dtobject,NULL, NULL, DTM_PROCLAYOUT, NULL,1);
      	DoMethod(dtobject,DTM_PROCLAYOUT,NULL,1L); /* Doesn't lock up the system while remapping */

		GetDTAttrs(dtobject, PDTA_BitMap, &bm1,
			PDTA_ColorTable, &pentable, PDTA_Allocated, &numpens, TAG_DONE);


		if(bmh->bmh_Masking==mskHasTransparentColor){
			long x,y;

			tmpRp1->BitMap=bm1;

			SetABPenDrMd(tmpRp1,0,1,JAM1);

			for(x=0;x<bmh->bmh_Width;x++)
				for(y=0;y<bmh->bmh_Height;y++)
					if(ReadPixel(tmpRp2,x,y)!=1)
						WritePixel(tmpRp1,x,y);

			if(tmpRp1) free(tmpRp1);
			if(tmpRp2) free(tmpRp2);

			}

		img_data->width = bmh->bmh_Width;
		img_data->height = bmh->bmh_Height;
		img_data->image_data = (char *)1;
		img_data->image = bm1;
		img_data->dt = dtobject;
		img_data->colors = pentable;
		img_data->fname = fnam;

		mo_cache_data (src, (void *)img_data, 0);
		}
 else {
      FILE *fp;
      unsigned char *data;
      int width, height, colrs;

      fp = fopen(fnam, "r");

      data = ReadXbmBitmap(fp, fnam, &width, &height, &colrs);
      fclose(fp);
      if (data) {
	bm1 = CreateBitMapFromXBMData(data, width, height,
 				      WIDG_DEPTH);
	img_data->width = width;
	img_data->height = height;
	img_data->image_data = (char *)1;
	img_data->image = bm1;
	img_data->dt = NULL;
	img_data->colors = NULL;
	img_data->fname = NULL;
	img_data->mask_bitmap=NULL;
      
	mo_cache_data (src, (void *)img_data, 0);

	unlink(fnam);
	free (fnam);

      } else {

	unlink(fnam);
	free (fnam);
	free(img_data);
	return 0;		/* datatype conversion failed! */
      }
    }      

  return 1;
}

/*************** INSTANCE HANDLING (METHODS) *************/
/*
** AskMinMax method will be called before the window is opened
** and before layout takes place. We need to tell MUI the
** minimum, maximum and default size of our object.
*/

static IPTR HTMLTextClAskMinMax(Class *cl, Object *obj, struct MUIP_AskMinMax *msg){
  /*
   ** let our superclass first fill in what it thinks about sizes.
   ** this will e.g. add the size of frame and inner spacing.
   */
  DoSuperMethodA(cl,obj,(Msg)msg);
  /*
   ** now add the values specific to our object. note that we
   ** indeed need to *add* these values, not just set them!
   */
  
  msg->MinMaxInfo->MinWidth  += 9000;
  msg->MinMaxInfo->DefWidth  += 9000;
  msg->MinMaxInfo->MaxWidth  += 9000;  /* There is an unlimited val, use it*/

  msg->MinMaxInfo->MinHeight += 9000;
  msg->MinMaxInfo->DefHeight += 9000;
  msg->MinMaxInfo->MaxHeight += 9000; /* same here */

  return(0);
}

static IPTR HTMLTextClSetup(Class *cl, Object *obj, Msg msg){
  if (!DoSuperMethodA(cl,obj,msg))
   return FALSE;
  MUI_RequestIDCMP(obj, IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY);
  return TRUE;
}

static IPTR HTMLTextClCleanup( Class *cl, Object *obj, Msg msg){
	MUI_RejectIDCMP(obj,IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY);
	return (DoSuperMethodA(cl,obj,msg));
}  

static IPTR HTMLTextClSet(Class *cl, Object *obj, Msg msg){
	HTMLTextClData *inst = INST_DATA(cl, obj);
	const struct TagItem *tstate;
	struct TagItem *ti;

	ti = ((opSetP)msg)->ops_AttrList;
	tstate = ti;
  
	while ((ti = NextTagItem(&tstate))) {
    switch (ti->ti_Tag) {
      
//    case HTMLA_active:		inst->active = ti->ti_Data; break;
//    case HTMLA_anchor_fg:	inst->anchor_fg = ti->ti_Data; break;
//    case HTMLA_visited_fg:	inst->visitedAnchor_fg = ti->ti_Data; break;
//    case HTMLA_active_fg:	inst->activeAnchor_fg = ti->ti_Data; break;
	case HTMLA_HTML_Data:   inst->HTML_Data = (HTMLGadClData *)ti->ti_Data; break;
      
    case HTMLA_scroll_y:
/* Take care of the virtual group here */ /* OBSOLETE? */
/*		kprintf("HTML Virt gad: Getting a new Y scroll: %ld\n",ti->ti_Data); */
		inst->new_scroll_value=ti->ti_Data;
		inst->new_scroll_dir=0;
		MUI_Redraw(obj,MADF_DRAWUPDATE); /* */
		break;
      
    case HTMLA_scroll_x:
/* Take care of the virtual group here */ /* OBSOLETE? */
/*		kprintf("HTML Virt gad: Getting a new X scroll: %ld\n",ti->ti_Data); */
		inst->new_scroll_value=ti->ti_Data;
		inst->new_scroll_dir=1;
		MUI_Redraw(obj,MADF_DRAWUPDATE); /* */
		break;

    case HTMLA_Top:
/* Take care of the virtual group here */
#if DEBUG0==1
		kprintf("HTML Virt gad: Getting a new Y position: %ld\n",ti->ti_Data);
#endif
		inst->new_scroll_value=ti->ti_Data;
		inst->new_scroll_dir=0;
		inst->Flags=1; /* 1= 'DrawUpdate' */
#if DEBUG0==1
		kprintf("Setting Virtgroup top: %ld\n",ti->ti_Data);
#endif

		set(G_Virt,MUIA_Virtgroup_Top,ti->ti_Data);
#if DEBUG0==1
		get(G_Virt,MUIA_Virtgroup_Top,&test);
		kprintf("The virtgroup got the top value: %ld\n",test);
#endif
		inst->Flags=0; /* Done 'DrawUpdate' */
//		MUI_Redraw(obj,MADF_DRAWUPDATE); /* Is done by VirtualGroup */
		break;
      
    case HTMLA_Left:
/* Take care of the virtual group here */
#if DEBUG0==1
		kprintf("HTML Virt gad: Getting a new X position: %ld\n",ti->ti_Data);
#endif
		inst->new_scroll_value=ti->ti_Data;
		inst->new_scroll_dir=1;
		inst->Flags=1; /* 1= 'DrawUpdate' */
#if DEBUG0==1
		kprintf("Setting Virtgroup left: %ld\n",ti->ti_Data);
#endif
		set(G_Virt,MUIA_Virtgroup_Left,ti->ti_Data);
#if DEBUG0==1
		get(G_Virt,MUIA_Virtgroup_Left,&test);
		kprintf("The virtgroup got the left value: %ld\n",test);
#endif
		inst->Flags=0; /* Done 'DrawUpdate' */
//		MUI_Redraw(obj,MADF_DRAWUPDATE); /* Is done by VirtualGroup */
		break;

    }
  }
  return DoSuperMethodA(cl, obj, msg);
}


/*------------------------------------------------------------------------
  Draw the HTML Gadget
  ------------------------------------------------------------------------*/
static IPTR HTMLTextClDraw(Class *cl, Object *obj, struct MUIP_Draw *msg){
	HTMLTextClData *inst = INST_DATA(cl, obj);

	DoSuperMethodA(cl,obj,(Msg)msg);

//	if(!(msg->flags & MADF_DRAWOBJECT) && !(msg->flags & MADF_DRAWUPDATE))
//		return 0L;

	if((msg->flags & MADF_DRAWOBJECT) && (inst->Flags & 1)){ /* Redraw exposed area */

		int bar=inst->new_scroll_dir,value=inst->new_scroll_value;
		HTMLGadClData *HTML_Data = inst->HTML_Data;
		/*
		 * If we've moved the vertical scrollbar
		 */

		if (bar == 0){
			/*
			 * We've scrolled down. Copy up the untouched part of the
			 * window.  Then Clear and redraw the new area
			 * exposed.
			 */
			if (value > HTML_Data->scroll_y){
				int dy;
	    
				dy = value - HTML_Data->scroll_y;
				if (dy > HTML_Data->view_height){
					HTML_Data->scroll_y = value;
//					XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, 0,HTML_Data->view_width,HTML_Data->view_height, False);
					ViewRedisplay(HTML_Data,0, 0,HTML_Data->view_width-1,HTML_Data->view_height);
					}
				else{
//					XCopyArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),XtWindow(HTML_Data->view),HTML_Data->drawGC, 0, dy,HTML_Data->view_width,HTML_Data->view_height - dy,0, 0);
					HTML_Data->scroll_y = value;
//					XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, (int)HTML_Data->view_height - dy,HTML_Data->view_width, dy, False);
					ViewRedisplay(HTML_Data,0, (int)HTML_Data->view_height - dy,HTML_Data->view_width-1, dy);
					}
				}
			/*
			 * We've scrolled up. Copy down the untouched part of the
			 * window.  Then Clear and redraw the new area
			 * exposed.
			 */
			else if (value < HTML_Data->scroll_y){
				int dy;
	    
				dy = HTML_Data->scroll_y - value;
				if (dy > HTML_Data->view_height){
					HTML_Data->scroll_y = value;
//					XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, 0,HTML_Data->view_width,HTML_Data->view_height, False);
					ViewRedisplay(HTML_Data,0, 0,HTML_Data->view_width-1,HTML_Data->view_height);
					}
				else{
//					XCopyArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),XtWindow(HTML_Data->view),HTML_Data->drawGC, 0, 0,HTML_Data->view_width,HTML_Data->view_height - dy,0, dy);
					HTML_Data->scroll_y = value;
//					XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, 0,HTML_Data->view_width, dy, False);
					ViewRedisplay(HTML_Data,0, 0,HTML_Data->view_width-1, dy);
					}
				}
			}
		/*
		 * Else we've moved the horizontal scrollbar
		 */
		else if (bar == 1){
			/*
			 * We've scrolled right. Copy left the untouched part of the
			 * window.  Then Clear and redraw the new area
			 * exposed.
			 */
			if (value > HTML_Data->scroll_x){
				int dx;
	    
				dx = value - HTML_Data->scroll_x;
				if (dx > HTML_Data->view_width){
					HTML_Data->scroll_x = value;
//					XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, 0,HTML_Data->view_width,HTML_Data->view_height-1, False);
					ViewRedisplay(HTML_Data,0, 0,HTML_Data->view_width,HTML_Data->view_height-1);
				}
				else{
//					XCopyArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),XtWindow(HTML_Data->view),HTML_Data->drawGC, dx, 0,HTML_Data->view_width - dx,HTML_Data->view_height-1,0, 0);
					HTML_Data->scroll_x = value;
//					XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),(int)HTML_Data->view_width - dx, 0,dx, HTML_Data->view_height-1, False);
					ViewRedisplay(HTML_Data,(int)HTML_Data->view_width - dx, 0,dx, HTML_Data->view_height-1);
				}
			}
			/*
			 * We've scrolled left. Copy right the untouched part of the
			 * window.  Then Clear and redraw the new area
			 * exposed.
			 */
			else if (value < HTML_Data->scroll_x){
				int dx;
	    
				dx = HTML_Data->scroll_x - value;
				if (dx > HTML_Data->view_width){
					HTML_Data->scroll_x = value;
//					XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, 0,HTML_Data->view_width,HTML_Data->view_height-1, False);
					ViewRedisplay(HTML_Data,0, 0,HTML_Data->view_width,HTML_Data->view_height-1);
					}
				else{
//					XCopyArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),XtWindow(HTML_Data->view),HTML_Data->drawGC, 0, 0,HTML_Data->view_width - dx,HTML_Data->view_height-1,dx, 0);
					HTML_Data->scroll_x = value;
//					XClearArea(XtDisplay(HTML_Data->view),XtWindow(HTML_Data->view),0, 0,dx, HTML_Data->view_height-1, False);
					ViewRedisplay(HTML_Data,0, 0,dx, HTML_Data->view_height-1);
					}
				}
			}
			inst->Flags=0;
		}
	else
	if((msg->flags & MADF_DRAWOBJECT) && (!inst->HTML_Data->reparsing)) /* total redraw */
		ViewClearAndRefresh(inst->HTML_Data);

//	DoSuperMethodA(cl,obj,(Msg)msg);

	return 0;
}

static IPTR HTMLTextClHandleInput(Class *cl, Object *obj, struct MUIP_HandleInput *msg)
{
	DoSuperMethodA(cl,obj,(Msg)msg);
//	kprintf("Getting input msg\n");
	HandleInput(msg);
	return 0;
}

/********************************************************
  The main HTMLText handler routine
********************************************************/
DISPATCHER(HTMLTextClDispatch)
{
	switch (msg->MethodID) { /* approx calling order */
		case MUIM_AskMinMax  : return(HTMLTextClAskMinMax(cl,obj,(APTR)msg));
		case MUIM_Draw       : return(HTMLTextClDraw(cl,obj,(APTR)msg));
		case OM_SET          : return(HTMLTextClSet(cl,obj,(APTR)msg));
//		case OM_NEW          : return(HTMLTextClNew(cl,obj,(APTR)msg));
//		case OM_DISPOSE      : return(HTMLTextClDispose(cl,obj,(APTR)msg));
		case MUIM_Setup      : return(HTMLTextClSetup(cl,obj,(APTR)msg));
//		case MUIM_Show       : return(HTMLTextClShow(cl,obj,(APTR)msg));
		case MUIM_HandleInput: return(HTMLTextClHandleInput(cl,obj,(APTR)msg));
//		case OM_GET          : return(HTMLTextClGet(cl,obj,(APTR)msg));
		case MUIM_Cleanup    : return(HTMLTextClCleanup(cl,obj,(APTR)msg)); 

  /* What follows is a set of methods for doing things that used to
     be done illegally by messing with the HTMLGadget from outside it.
     They are called by cover functions in TO_HTML.c (and maybe 
     other places too, since this is legal). Note that we will
     usually have to use "set" immdiately before calling these
     methods, to prepare the structures they use. */ 

//  case HTMLM_SetText   : return(HTMLGadClSetText(cl,obj,(APTR)msg));
//  case HTMLM_ResetAmigaGadgets : return(HTMLGadClResetAmigaGadgets(cl,obj,(APTR)msg));
/* Methods to be implemented:
	HTMLM_LoadInlinedImages:
	HTMLM_LoadInlinedImage:
    HTMLM_ClearImageCache:
    HTMLM_GotoAnchor:
*/

  }
  return(DoSuperMethodA(cl,obj,msg));
}

/*********************  HTMLTextN  *********************/


/*************** INSTANCE HANDLING (METHODS) *************/
/*
** AskMinMax method will be called before the window is opened
** and before layout takes place. We need to tell MUI the
** minimum, maximum and default size of our object.
*/

static IPTR HTMLTextNClAskMinMax(Class *cl, Object *obj, struct MUIP_AskMinMax *msg){
  /*
   ** let our superclass first fill in what it thinks about sizes.
   ** this will e.g. add the size of frame and inner spacing.
   */
  DoSuperMethodA(cl,obj,(Msg)msg);
  /*
   ** now add the values specific to our object. note that we
   ** indeed need to *add* these values, not just set them!
   */
  
  msg->MinMaxInfo->MinWidth  += 20;
  msg->MinMaxInfo->DefWidth  += 300;
  msg->MinMaxInfo->MaxWidth  += 9000;  /* There is an unlimited val, use it*/

  msg->MinMaxInfo->MinHeight += 20;
  msg->MinMaxInfo->DefHeight += 350;
  msg->MinMaxInfo->MaxHeight += 9000; /* same here */

  return(0);
}

static IPTR HTMLTextNClSetup(Class *cl, Object *obj, Msg msg){
  if (!DoSuperMethodA(cl,obj,msg))
   return FALSE;
  MUI_RequestIDCMP(obj, IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY);
  return TRUE;
}

static IPTR HTMLTextNClCleanup( Class *cl, Object *obj, Msg msg){
	MUI_RejectIDCMP(obj,IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY);
	return (DoSuperMethodA(cl,obj,msg));
}  
static IPTR HTMLTextNClSet(Class *cl, Object *obj, Msg msg){
	HTMLTextClData *inst = INST_DATA(cl, obj);
	const struct TagItem *tstate;
	struct TagItem *ti;
  
	ti = ((opSetP)msg)->ops_AttrList;
	tstate = ti;
  
	while ((ti = NextTagItem(&tstate))) {
    switch (ti->ti_Tag) {
      
//    case HTMLA_active:		inst->active = ti->ti_Data; break;
//    case HTMLA_anchor_fg:	inst->anchor_fg = ti->ti_Data; break;
//    case HTMLA_visited_fg:	inst->visitedAnchor_fg = ti->ti_Data; break;
//    case HTMLA_active_fg:	inst->activeAnchor_fg = ti->ti_Data; break;
	case HTMLA_HTML_Data:   inst->HTML_Data = (HTMLGadClData *)ti->ti_Data; break;
      
    case HTMLA_Top:
#if DEBUG0==1
		kprintf("HTML Text gad: Getting a new Y position: %ld\n",ti->ti_Data);
#endif
		inst->new_scroll_value=ti->ti_Data;
		inst->new_scroll_dir=0;
		MUI_Redraw(obj,MADF_DRAWUPDATE);
		break;
      
    case HTMLA_Left:
#if DEBUG0==1
		kprintf("HTML Text gad: Getting a new X position: %ld\n",ti->ti_Data);
#endif
		inst->new_scroll_value=ti->ti_Data;
		inst->new_scroll_dir=1;
		MUI_Redraw(obj,MADF_DRAWUPDATE);
		break;
    }
  }
  return DoSuperMethodA(cl, obj, msg);
}

static IPTR HTMLTextNClDraw(Class *cl, Object *obj, struct MUIP_Draw *msg){
	HTMLTextClData *inst = INST_DATA(cl, obj);

	DoSuperMethodA(cl,obj,(Msg)msg);

	if(!(msg->flags & MADF_DRAWOBJECT) && !(msg->flags & MADF_DRAWUPDATE))
		return 0L;

	if(msg->flags & MADF_DRAWUPDATE) /* scroll to position */
		ScrollToPos(inst->HTML_Data, inst->new_scroll_dir, inst->new_scroll_value);

	if((msg->flags & MADF_DRAWOBJECT) && !inst->HTML_Data->reparsing) /* total redraw */
		{
		/* This is necessary in case the HTMLSetText was called before the gadget
		   was ready, i.e. after a window resize.  This is MUI's fault (still!) */
  
		if (inst->HTML_Data->html_objects == NULL) {
//    NotifyChange(cl, (Object *)g, (struct gpInput *)msg, HTML_mui_resize, (IPTR)NULL);
			DoMethod(HTML_Gad,HTMLM_ResetAmigaGadgets); /* Hope this is correct */
			HTMLSetText(inst->HTML_Data, inst->HTML_Data->raw_text, NULL, NULL, 0, NULL, NULL);
 			}

/*  ReformatWindow(HTML_Data); */ /* This is done in show */
	ViewClearAndRefresh(inst->HTML_Data);
		}
	return 0;
}

static IPTR HTMLTextNClHandleInput(Class *cl, Object *obj, struct MUIP_HandleInput *msg)
{
	DoSuperMethodA(cl,obj,(Msg)msg);
	HandleInput(msg);
	return 0;
}

/********************************************************
  The main HTMLTextN handler routine
********************************************************/
DISPATCHER(HTMLTextNClDispatch)
{
   switch (msg->MethodID) {
	case MUIM_AskMinMax  : return(HTMLTextNClAskMinMax(cl,obj,(APTR)msg));
	case MUIM_Draw       : return(HTMLTextNClDraw(cl,obj,(APTR)msg));
	case OM_SET          : return(HTMLTextNClSet(cl,obj,(APTR)msg));
	case MUIM_Setup      : return(HTMLTextNClSetup(cl,obj,(APTR)msg));
	case MUIM_Cleanup    : return(HTMLTextNClCleanup(cl,obj,(APTR)msg));
	case MUIM_HandleInput: return(HTMLTextNClHandleInput(cl,obj,(APTR)msg));

//		case MUIM_Show       : return(HTMLTextNClShow(cl,obj,(APTR)msg));
  }
  return(DoSuperMethodA(cl,obj,msg));
}

/*****************************************************
 * All functions below deals with forms, gadgets etc *
 *                                                   *
 * Notes:                                            *
 *    Everything is under construction, do not asume *
 *    anything about it.                             *
 *                                                   *
 * Bugs:                                             *
 *    Cannot handle History moves                    *
 *    Doesn't free everything after itself           *
 *                                                   *
 *****************************************************/

WidgetInfo *
FindWidget(WidgetInfo *w,long Id)
{
	for(;w;w=w->next)
		if(w->id==Id)
			return w;
	return NULL;
}

char **SplitOptions(char *in_str){
	char *tptr,**out,**tout,*new;
	long opt,ns;
	new=malloc(strlen(in_str)+1);

	strcpy(new,in_str);

	opt=0;
	for(tptr=new;*tptr;tptr++)
		if(*tptr=='|')
			opt++;
	if(*in_str)
		opt++;

	out=malloc(sizeof(char *)*(opt+1));
	ns=1;
	for(tptr=new,tout=out;*tptr;tptr++){
		if(ns){
			*tout++=tptr;
			ns=0;
			}
		if(*tptr=='|'){
			*tptr=0x0;
			ns=1;
			}
		}
	*tout=NULL;
	return out;
}

#define FTYPE_NOTSET   0   /* Just to know if we have done anything or not */
#define FTYPE_TEXT     1   /* These are just preliminary types */
#define FTYPE_TEXTAREA 2
#define FTYPE_LISTV    3
#define FTYPE_ISINDEX  4
#define FTYPE_CYCLE    5
#define FTYPE_HIDDEN   6   /* Just some text to send to the cgi-bin */
#define FTYPE_BUTTON_S 7   /* Button submit */
#define FTYPE_BUTTON_R 11  /* Button reset */
#define FTYPE_BUTTON_C 13  /* Button clear */
#define FTYPE_PASSWORD 8
#define FTYPE_CHECKBOX 9
#define FTYPE_RADIO    10
#define FTYPE_SELECT   12  /* This is both a cycle and a list view */
#define FTYPE_UNKNOWN 999 /* I hope it never will be needed */

/*
radio     100% working
checkbox  100% working
text      100% working
password  100% working
submit    100% working
reset     100% working
hidden    100% working

listv     100% working
cycle     100% working

textarea   20% working
isindex   100%
*/

ULONG ParseWidgetType(char *type)
{
	if(type){ /* Make sure it isn't a NULL pointer */
		if(!strnicmp(type,"hidden",7))
			return FTYPE_HIDDEN;
		if(!strnicmp(type,"text",5))
			return FTYPE_TEXT;
		if(!strnicmp(type,"password",9))
			return FTYPE_PASSWORD;
		if(!strnicmp(type,"checkbox",9))
			return FTYPE_CHECKBOX;
		if(!strnicmp(type,"radio",6))
			return FTYPE_RADIO;
		if(!strnicmp(type,"textarea",9))
			return FTYPE_TEXTAREA;
		if(!strnicmp(type,"select",7))
			return FTYPE_SELECT;

		if(!strnicmp(type,"submit",7))
			return FTYPE_BUTTON_S;
		if(!strnicmp(type,"reset",6))
			return FTYPE_BUTTON_R;
		if(!strnicmp(type,"clear",6))
			return FTYPE_BUTTON_C;
		}
	return FTYPE_UNKNOWN; /* The stupid author of the page has left the type out. */
}                         /* Now I have to determine it by myself. */

ULONG GetTextWidth(char **text)
{
	if(text){
		ULONG size=0,l;
		while(*text){
			l=strlen(*text);
			if(l>size)
				size=l;
			text++;
			}
		return size;
		}
	return 0;
}

WidgetInfo *
MakeWidget(HTMLGadClData *HTML_Data, char *edata,
				long x,long y,long WidgetId,FormInfo *Form)
{
	WidgetInfo *widget,*cw;
	char *tptr;
	char *type,*value,*name,*options;
	long width,height;

//	kprintf("Called MakeWidget\n");
//	kprintf("FormInfo : action: %s method: %s\n",Form->action,Form->method);
//	kprintf("Edata: -%s-, WidgetId: %ld\n",edata,WidgetId);
//	kprintf("Cords x: %ld y: %ld\n",x,y);

	if(HTML_Data->object_flags & Object_Reparsing){
		if((widget=FindWidget(HTML_Data->widget_list,WidgetId))){
			widget->flags=0;
			widget->ParentForm=Form; /* To have a link back */
			SetAttrs(widget->MUI_Object,
				MUIA_NewGroup_Left,x+mui_add(HTML_Data->view_left-HTML_Data->scroll_x),
				MUIA_NewGroup_Top,y+mui_add(HTML_Data->view_top-HTML_Data->scroll_y),TAG_DONE);
			GetAttr(MUIA_NewGroup_Width,widget->MUI_Object,(IPTR *)&widget->width);
			GetAttr(MUIA_NewGroup_Height,widget->MUI_Object,(IPTR *)&widget->height);
			}
		return widget;
		}

	if((widget=FindWidget(HTML_Data->widget_list,WidgetId))){
		/* If this is true the old widget has just got some new cordinates */
		widget->flags=0;
		widget->ParentForm=Form; /* To have a link back */
		SetAttrs(widget->MUI_Object,
			MUIA_NewGroup_Left,x+mui_add(HTML_Data->view_left),
			MUIA_NewGroup_Top,y+mui_add(HTML_Data->view_top),TAG_DONE);
			return widget;
		}		

	widget=calloc(1,sizeof(WidgetInfo));

	type=ParseMarkTag(edata,MT_INPUT,"TYPE");
	value=NULL;

	if((widget->type=ParseWidgetType(type))==FTYPE_UNKNOWN){
		/* Try to find out what sort of input tag this is */
		widget->type=FTYPE_TEXT; /* Default is text entry */
		}

	name=ParseMarkTag(edata,MT_INPUT,"NAME");
	height=14;
	width=64;

	switch(widget->type){
		case FTYPE_BUTTON_S:
		case FTYPE_BUTTON_R:
		case FTYPE_BUTTON_C:
			value=ParseMarkTag(edata,MT_INPUT,"VALUE");
			if(!value || *value==0x0){
			
				if(widget->type==FTYPE_BUTTON_S)
					value=strdup("Submit");
				else
					if(widget->type==FTYPE_BUTTON_R)
						value=strdup("Reset");}
			else{
				if(!strnicmp(value,"clear",5))
					widget->type=FTYPE_BUTTON_C;
				}
			widget->MUI_Object=NewObject(NewGroupClass,NULL,
									MUIA_NewGroup_ChildWidth,TRUE,
									MUIA_NewGroup_ChildHeight,TRUE,
									MUIA_NewGroup_Left,x+mui_add(HTML_Data->view_left),
									MUIA_NewGroup_Top,y+mui_add(HTML_Data->view_top),
									Child,widget->MUI_SubObject=SimpleButton(value),
									TAG_DONE);
			DoMethod(widget->MUI_SubObject,MUIM_Notify,MUIA_Pressed,FALSE,
					widget->MUI_SubObject,4,MUIM_CallHook,&FormButton_hook,widget,Form);
			if(widget->type==FTYPE_BUTTON_S && name){
//				value=name;
//				name=strdup("submit");
				free(value);
				value=strdup("submit");
				}
			width=strlen(value)*8+8;
			break;
		case FTYPE_TEXT:
		case FTYPE_PASSWORD:{
			LONG size,maxsize;
			tptr=ParseMarkTag(edata,MT_INPUT,"SIZE");
			if(tptr)
				size=atoi(tptr);
			else
				size=20;
			free(tptr);
			tptr=ParseMarkTag(edata,MT_INPUT,"MAXLENGTH");
			if(tptr)
				maxsize=atoi(tptr)+1; /* +1 For the NULL byte */
			else
				maxsize=256;
			free(tptr);
			value=ParseMarkTag(edata,MT_INPUT,"VALUE");

			if(widget->type==FTYPE_TEXT)
				widget->MUI_Object=NewObject(NewGroupClass,NULL,
								MUIA_NewGroup_Width,size*8+4,
								MUIA_NewGroup_ChildHeight,TRUE,
								MUIA_NewGroup_Left,x+mui_add(HTML_Data->view_left),
								MUIA_NewGroup_Top,y+mui_add(HTML_Data->view_top),
								Child,widget->MUI_SubObject=StringObject,StringFrame,
												MUIA_String_MaxLen,maxsize,
								End,
							TAG_DONE);
			else
				widget->MUI_Object=NewObject(NewGroupClass,NULL,
								MUIA_NewGroup_Width,size*8+4,
								MUIA_NewGroup_ChildHeight,TRUE,
								MUIA_NewGroup_Left,x+mui_add(HTML_Data->view_left),
								MUIA_NewGroup_Top,y+mui_add(HTML_Data->view_top),
								Child,widget->MUI_SubObject=StringObject,StringFrame,
												MUIA_String_MaxLen,maxsize,
												MUIA_String_Secret,TRUE,End,
							TAG_DONE);
			if(value)
				set(widget->MUI_SubObject,MUIA_String_Contents,value);
			height=11;
			width=size*8+4;

			break;}
		case FTYPE_HIDDEN:{
			value=ParseMarkTag(edata,MT_INPUT,"VALUE");
			width=0;
			height=0;
			break;}
		case FTYPE_TEXTAREA:{
			LONG ccols,crows;
			tptr=ParseMarkTag(edata,MT_INPUT,"COLS");
			if(tptr) ccols=atoi(tptr);
			else ccols=10; /* This should never happen */
			free(tptr);
			tptr=ParseMarkTag(edata,MT_INPUT,"ROWS");
			if(tptr) crows=atoi(tptr);
			else crows=10; /* This should never happen */
			free(tptr);

			tptr=ParseMarkTag(edata,MT_INPUT,"value");
			widget->MUI_Object=NewObject(NewGroupClass,NULL,
						MUIA_NewGroup_ChildWidth,TRUE,
						MUIA_NewGroup_ChildHeight,TRUE,
						MUIA_NewGroup_Left,x+mui_add(HTML_Data->view_left),
						MUIA_NewGroup_Top,y+mui_add(HTML_Data->view_top),
						Child,widget->MUI_SubObject=NewObject(TextEditFieldClass,NULL,
							MUIA_TEF_Width,ccols,MUIA_TEF_Height,crows,
							MUIA_TEF_Text,tptr,
							MUIA_TEF_Active,FALSE,
							TAG_DONE),
						TAG_DONE);
			DoMethod(widget->MUI_SubObject,MUIM_Notify,MUIA_TEF_Active,MUIV_EveryTime,
					widget->MUI_SubObject,2,MUIM_CallHook,&TextEdit_hook);
			width=ccols*8+24;
			height=crows*8+16;
			break;}
		case FTYPE_CHECKBOX:{
			value=ParseMarkTag(edata,MT_INPUT,"VALUE");
			if(!value)
				value=strdup("on");
			widget->MUI_Object=NewObject(NewGroupClass,NULL,
						MUIA_NewGroup_ChildWidth,TRUE,
						MUIA_NewGroup_ChildHeight,TRUE,
						MUIA_NewGroup_Left,x+mui_add(HTML_Data->view_left),
						MUIA_NewGroup_Top,y+mui_add(HTML_Data->view_top),
						Child,widget->MUI_SubObject=ImageObject,ButtonFrame,MUIA_InputMode,MUIV_InputMode_Toggle,
							MUIA_Image_FreeHoriz,TRUE,MUIA_Image_FreeVert,TRUE,
							MUIA_Image_Spec,MUII_CheckMark,
							MUIA_Background,MUII_BACKGROUND,
							End,
						TAG_DONE);
			if((tptr=ParseMarkTag(edata,MT_INPUT,"CHECKED"))){
				set(widget->MUI_SubObject,MUIA_Selected,TRUE);
				widget->checked=TRUE;
				free(tptr);
				}
			width=24;
			height=16;
			break;}
		case FTYPE_RADIO:{
			value=ParseMarkTag(edata,MT_INPUT,"VALUE");
			if(!value)
				value=strdup("on");
			widget->MUI_Object=NewObject(NewGroupClass,NULL,
						MUIA_NewGroup_ChildWidth,TRUE,
						MUIA_NewGroup_ChildHeight,TRUE,
						MUIA_NewGroup_Left,x+mui_add(HTML_Data->view_left),
						MUIA_NewGroup_Top,y+mui_add(HTML_Data->view_top),
						Child,widget->MUI_SubObject=ImageObject,MUIA_InputMode,MUIV_InputMode_Immediate,
							MUIA_Image_FreeHoriz,TRUE,MUIA_Image_FreeVert,TRUE,
							MUIA_Image_Spec,MUII_RadioButton,
							MUIA_Background,MUII_BACKGROUND,
							End,
						TAG_DONE);
			if((tptr=ParseMarkTag(edata,MT_INPUT,"CHECKED"))){
				set(widget->MUI_SubObject,MUIA_Selected,TRUE);
				free(tptr);
				widget->checked=TRUE;
				}
			DoMethod(widget->MUI_SubObject,MUIM_Notify,MUIA_Selected,TRUE,
					widget->MUI_SubObject,3,MUIM_CallHook,&FormRadio_hook,widget);

			width=16;
			height=10;
			break;}
		case FTYPE_SELECT:{
			char **cyclelist,**selectedlist,**returnlist, *returns,*selected;
			long i,n;
			tptr=ParseMarkTag(edata,MT_INPUT,"MULTIPLE");
			if(tptr){
			widget->type=FTYPE_LISTV; /* Somewhat dirty to change the type on the fly, but quite handy. */
			selected=ParseMarkTag(edata,MT_INPUT,"VALUE");
			selectedlist=SplitOptions(selected);

//			kprintf("Selected %s\n",selected);

			options=ParseMarkTag(edata,MT_INPUT,"OPTIONS");
			cyclelist=SplitOptions(options);

			returns=ParseMarkTag(edata,MT_INPUT,"RETURNS");
			returnlist=SplitOptions(returns);

/*			selectedlist=SplitOpions(selected); */

			widget->option=(char *)cyclelist;
			widget->returnv=(char *)returnlist;
			value=(char *)selectedlist;			

			widget->MUI_Object=NewObject(NewGroupClass,NULL,
						MUIA_NewGroup_Width,128,
						MUIA_NewGroup_Height,48,
						MUIA_NewGroup_Left,x+mui_add(HTML_Data->view_left),
						MUIA_NewGroup_Top,y+mui_add(HTML_Data->view_top),
						Child,ListviewObject,
							MUIA_Listview_MultiSelect,MUIV_Listview_MultiSelect_Def,
							MUIA_Listview_List,widget->MUI_SubObject=ListObject,
								InputListFrame,
								MUIA_List_SourceArray, cyclelist,
								End,
							End,
						TAG_DONE);
			for(i=0;cyclelist[i];i++)
				for(n=0;selectedlist[n];n++)
					if(!stricmp(cyclelist[i],selectedlist[n]))
						DoMethod(widget->MUI_SubObject,MUIM_List_Select,i,MUIV_List_Select_On,NULL);

			width=128;
			height=64;
			}
			else
			{
			widget->type=FTYPE_CYCLE; /* Somewhat dirty to change the type on the fly, but quite handy. */
			value=ParseMarkTag(edata,MT_INPUT,"VALUE");
			options=ParseMarkTag(edata,MT_INPUT,"OPTIONS");
			cyclelist=SplitOptions(options);

			returns=ParseMarkTag(edata,MT_INPUT,"RETURNS");
			returnlist=SplitOptions(returns);

			for(n=0;cyclelist[n];n++)
				if(!stricmp(cyclelist[n],value))
					break;

			widget->option=(char *)cyclelist;
			widget->returnv=(char *)returnlist;
			widget->MUI_Object=NewObject(NewGroupClass,NULL,
						MUIA_NewGroup_ChildWidth,TRUE,
						MUIA_NewGroup_ChildHeight,TRUE,
						MUIA_NewGroup_Left,x+mui_add(HTML_Data->view_left),
						MUIA_NewGroup_Top,y+mui_add(HTML_Data->view_top),
						Child,widget->MUI_SubObject=CycleObject,MUIA_Cycle_Entries,cyclelist,End,
						TAG_DONE);
			if(cyclelist[n])
				set(widget->MUI_Object,MUIA_Cycle_Active,n);


			width=GetTextWidth(cyclelist)*8+24;
			height=16;
			}
			break;}
		}

	widget->height=height;
	widget->width=width;
	widget->x=x;
	widget->y=y;
	widget->id=WidgetId;
	widget->name=name;
	widget->value=value;
	widget->flags=0;

	widget->ParentForm=Form; /* To have a link back */

	cw=HTML_Data->widget_list;
	if(cw){
		for(;cw->next;cw=cw->next);
		cw->next=widget;
		}
	else
		HTML_Data->widget_list=widget;	
	widget->next=NULL;

	return widget;
}

void AddNewForm(HTMLGadClData *HTML_Data,FormInfo *Form){
/* This should probably set up all fields that should be
   returned by the form etc */
	WidgetInfo *cw;
	FormPartInfo *fp,*lfp=NULL;

	Object *stringgadget=NULL;

//	kprintf("Called Add new form: %08lx\n",Form);

	Form->href=Form->action;

	for(cw=HTML_Data->widget_list;cw;cw=cw->next){
	if((cw->flags == 0) && (cw->ParentForm==Form)){
		fp=calloc(1,sizeof(FormPartInfo));
		cw->flags=1;

		fp->Name=cw->name;
		fp->Type=cw->type;
		fp->Widget=cw;
		if(lfp)
			lfp->Next=fp;
		else
			Form->FirstFormPart=fp;
		lfp=fp;
		
		switch(cw->type){
			case FTYPE_TEXT:
				fp->UserData=cw->MUI_SubObject;
				if(stringgadget==0)
					stringgadget=cw->MUI_SubObject;
				else
					stringgadget=(Object *)-1;
				break;
			case FTYPE_PASSWORD:
			case FTYPE_TEXTAREA:
			case FTYPE_CYCLE:
			case FTYPE_LISTV:
				fp->UserData=cw->MUI_SubObject;
				stringgadget=(Object *)-1;
			break;
			case FTYPE_HIDDEN:
				fp->UserData=cw;
				stringgadget=(Object *)-1;
			break;
		case FTYPE_CHECKBOX:
		{
				WidgetInfo *dw;
				long c=0;
				WidgetInfo **wlist;
				if(cw->name)
					for(dw=HTML_Data->widget_list;dw;dw=dw->next)
						if((dw->flags == 0) && (dw->ParentForm==Form) && (dw->type==FTYPE_CHECKBOX))
							if(dw->name)
								if(stricmp(dw->name,cw->name)==0)
									c++;
				wlist=malloc(c*4+8);
				fp->UserData=(APTR)wlist;
				*wlist++=cw;
				if(cw->name)
					for(dw=HTML_Data->widget_list;dw;dw=dw->next)
						if((dw->flags == 0) && (dw->ParentForm==Form) && (dw->type==FTYPE_CHECKBOX))
							if(dw->name)
								if(stricmp(dw->name,cw->name)==0){
									*wlist++=dw;
									dw->flags|=1; /* Done */
									}
				*wlist=NULL;}
				stringgadget=(Object *)-1;
			break;
		case FTYPE_RADIO:{
				WidgetInfo *dw;
				long c=0;
				WidgetInfo **wlist;
				if(cw->name)
					for(dw=HTML_Data->widget_list;dw;dw=dw->next)
						if((dw->flags == 0) && (dw->ParentForm==Form) && (dw->type==FTYPE_RADIO))
							if(dw->name)
								if(stricmp(dw->name,cw->name)==0)
									c++;
				wlist=malloc(c*4+8);
				fp->UserData=(APTR)wlist;
				*wlist++=cw;
				if(cw->name)
					for(dw=HTML_Data->widget_list;dw;dw=dw->next)
						if((dw->flags == 0) && (dw->ParentForm==Form) && (dw->type==FTYPE_RADIO))
							if(dw->name)
								if(stricmp(dw->name,cw->name)==0){
									*wlist++=dw;
									dw->flags|=1; /* Done */
									}
				*wlist=NULL;
				
				dw=NULL;
				for(wlist=(WidgetInfo **)fp->UserData;*wlist;wlist++){
					(*wlist)->PrevRadio=dw;
					(*wlist)->NextRadio=wlist[1]; /* Next element */
					dw=*wlist;
					}
				}
				stringgadget=(Object *)-1;
			break;
			}
		}
	}
	if(stringgadget && (stringgadget!=(Object *)-1)){
		DoMethod(stringgadget,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
				stringgadget,4,MUIM_CallHook,&FormString_hook,NULL,Form);
		}
}

void AddNewWidgets(HTMLGadClData *HTML_Data)
{
	WidgetInfo *cw;
	struct Window *win = NULL;

	if(HTML_Data->widget_list){
		if(!(HTML_Data->object_flags&Object_Changing)){
			HTML_Data->object_flags|=Object_Changing;

			if(mui_version<11){
				set(HTML_Gad,HTMLA_Top,0);
				set(HTML_Gad,HTMLA_Left,0);
				set(WI_Wait,MUIA_Window_Open,TRUE); /* Before WI_Main to keep mui from closing pubscreen */
				HTML_Data->have_formsI=1;
				set(WI_Main, MUIA_Window_Open, FALSE); /* To be able to add the objects */
				get(WI_Wait,MUIA_Window_Window,&win);
				HTML_Data->rp=win->RPort; /* This is VERY ugly! */ /* But it works :) */

//				kprintf("Closing window...\n");
				}
			else
				DoMethod(HTML_Gad,MUIM_Group_InitChange); /* To be able to add the objects */
			}
		set(HTML_Gad,MUIA_Group_ActivePage,1);
		for(cw=HTML_Data->widget_list;cw;cw=cw->next){
//			kprintf("Adding object...\n");
			DoMethod(G_Virt,OM_ADDMEMBER,cw->MUI_Object);
			}
		HTML_Data->have_forms=1;
//		DoMethod(App,MUIM_Application_SetMenuState,mo_expand_images_current,FALSE);
//		DoMethod(App,MUIM_Application_SetMenuState,mo_delay_image_loads,FALSE);
		DoMethod(App,MUIM_Application_SetMenuState,mo_clear_image_cache,FALSE);
		DoMethod(App,MUIM_Application_SetMenuState,mo_isindex_to_form,FALSE);

		if(mui_version<11){
			DoMethod(App,MUIM_Application_SetMenuState,mo_buttons_text,FALSE);
			DoMethod(App,MUIM_Application_SetMenuState,mo_buttons_image,FALSE);
			DoMethod(App,MUIM_Application_SetMenuState,mo_buttons_imagetext,FALSE);
			}
		}
}
void DisposeWidget(WidgetInfo *w)
{
	if(w->name)
		free(w->name);
	if(w->value)
		free(w->value);
	if(w->option)
		free(w->option);

	switch(w->type){
		case FTYPE_TEXT:
		case FTYPE_PASSWORD:
		case FTYPE_HIDDEN:
		case FTYPE_TEXTAREA:
			break;
		case FTYPE_CYCLE:
		case FTYPE_LISTV:
			if(w->returnv) free(w->returnv);
			break;
		case FTYPE_CHECKBOX:
		case FTYPE_RADIO:
		break;
		}

	free(w);
}

void DisposeForms(FormInfo *Form){
/* This should probably set up all fields that should be
   returned by the form etc */
	FormPartInfo *fp,*lfp=NULL;
	FormInfo *lfi;

	for(;Form;Form=lfi){
		for(fp=Form->FirstFormPart;fp;fp=lfp){
			switch(fp->Type){
				case FTYPE_TEXT:
				case FTYPE_PASSWORD:
				case FTYPE_TEXTAREA:
				case FTYPE_CYCLE:
				case FTYPE_LISTV:
				case FTYPE_HIDDEN:
					break;
				case FTYPE_CHECKBOX:
				case FTYPE_RADIO:
					if(fp->UserData) free(fp->UserData);
					break;
				}
				lfp=fp->Next;
				free(fp);
			}
		lfi=Form->next;
		free(Form);
		}
}

void HideWidgets(HTMLGadClData *HTML_Data)
{
	WidgetInfo *ow,*nw;
	struct Window *win = NULL;

	if(HTML_Data->widget_list){
		HTML_Data->object_flags|=Object_Changing;

		if(mui_version<11){
			set(HTML_Gad,HTMLA_Top,0);
			set(HTML_Gad,HTMLA_Left,0);
			set(WI_Wait,MUIA_Window_Open,TRUE); /* Before WI_Main to keep mui from closing pubscreen */
			HTML_Data->have_formsI=1;
			set(WI_Main, MUIA_Window_Open, FALSE); /* To be able to add the objects */
			get(WI_Wait,MUIA_Window_Window,&win);
			HTML_Data->rp=win->RPort; /* This is VERY ugly! */ /* But it works :) */

//			kprintf("Closing window...\n");
			}
		else
			DoMethod(HTML_Gad,MUIM_Group_InitChange); /* To be able to add the objects */

		for(ow=HTML_Data->widget_list;ow;ow=nw){
//			kprintf("Removing object...\n");
			DoMethod(G_Virt,OM_REMMEMBER,ow->MUI_Object);
			DisposeObject(ow->MUI_Object);
			nw=ow->next;
			DisposeWidget(ow);
			}
		HTML_Data->widget_list=NULL;
		}
}

void ShowHtml_RG(void)
{
	HTMLGadClData *HTML_Data=INST_DATA(HTMLGadClass,HTML_Gad);

	if(!window.binary_transfer){

	if(mui_version>=11){
		HTML_Data->scroll_y=HTML_Data->new_scroll_y;
		DoMethod(HTML_Gad,HTMLM_ResetAmigaGadgets);
		}
	else
		if(!HTML_Data->have_forms){
			DoMethod(HTML_Gad,HTMLM_ResetAmigaGadgets);
			}

	if(HTML_Data->object_flags & Object_Changing){
		if(!HTML_Data->widget_list){
//			DoMethod(App,MUIM_Application_SetMenuState,mo_expand_images_current,TRUE);
//			DoMethod(App,MUIM_Application_SetMenuState,mo_delay_image_loads,TRUE);
			DoMethod(App,MUIM_Application_SetMenuState,mo_clear_image_cache,TRUE);
			DoMethod(App,MUIM_Application_SetMenuState,mo_isindex_to_form,TRUE);

			if(mui_version<11){
				DoMethod(App,MUIM_Application_SetMenuState,mo_buttons_text,TRUE);
				DoMethod(App,MUIM_Application_SetMenuState,mo_buttons_image,TRUE);
				DoMethod(App,MUIM_Application_SetMenuState,mo_buttons_imagetext,TRUE);
				}
			HTML_Data->have_forms=0;
			set(HTML_Gad,MUIA_Group_ActivePage,0);
			}
		else{
//			DoMethod(App,MUIM_Application_SetMenuState,mo_expand_images_current,FALSE);
//			DoMethod(App,MUIM_Application_SetMenuState,mo_delay_image_loads,FALSE);
			DoMethod(App,MUIM_Application_SetMenuState,mo_clear_image_cache,FALSE);
			DoMethod(App,MUIM_Application_SetMenuState,mo_isindex_to_form,FALSE);

			if(mui_version<11){
				DoMethod(App,MUIM_Application_SetMenuState,mo_buttons_text,FALSE);
				DoMethod(App,MUIM_Application_SetMenuState,mo_buttons_image,FALSE);
				DoMethod(App,MUIM_Application_SetMenuState,mo_buttons_imagetext,FALSE);
				}
			HTML_Data->have_forms=1;
			}
		if(mui_version<11){
			set(WI_Main, MUIA_Window_Open, TRUE); /* Open the window again */
			HTML_Data->have_formsI=0;
			set(WI_Wait, MUIA_Window_Open, FALSE); /* Hide info */
//			kprintf("Opening window.\n");
			}
		else
			DoMethod(HTML_Gad,MUIM_Group_ExitChange); /* All objects are added */
		HTML_Data->object_flags&=~Object_Changing;
		}
	else
		MUI_Redraw(HTML_Gad,MADF_DRAWOBJECT);

	if((mui_version<11) /* && HTML_Data->have_forms */){
/*		HTML_Data->scroll_y=HTML_Data->new_scroll_y; */
				DoMethod(HTML_Gad,HTMLM_ResetAmigaGadgets);
		}
	}
}

void ClearWidget(FormPartInfo *fp)
{
	long n;
	switch(fp->Type){
		case FTYPE_TEXT:
		case FTYPE_PASSWORD:
			set((Object *)fp->UserData,MUIA_String_Contents,"");
			break;
		case FTYPE_TEXTAREA:
			set((Object *)fp->UserData,MUIA_TEF_Text,"");
			break;
		case FTYPE_CYCLE:
			set((Object *)fp->UserData,MUIA_Cycle_Active,0);
			break;
		case FTYPE_LISTV:
			DoMethod((Object *)fp->UserData,MUIM_List_Select,MUIV_List_Select_All,MUIV_List_Select_Off,NULL);
			break;
		case FTYPE_CHECKBOX:
			for(n=0;((WidgetInfo **)fp->UserData)[n];n++){
				set((((WidgetInfo **)fp->UserData)[n])->MUI_SubObject,MUIA_Selected,0);
				}
			break;
		case FTYPE_RADIO:
			for(n=0;((WidgetInfo **)fp->UserData)[n];n++){
				set((((WidgetInfo **)fp->UserData)[n])->MUI_SubObject,MUIA_Selected,0);
				}
			break;
		}
}

void ResetWidget(FormPartInfo *fp)
{
	long n = 0;
	switch(fp->Type){
		case FTYPE_TEXT:
		case FTYPE_PASSWORD:
			set((Object *)fp->UserData,MUIA_String_Contents,fp->Widget->value);
			break;
		case FTYPE_TEXTAREA:
			set((Object *)fp->UserData,MUIA_TEF_Text,fp->Widget->value);
			break;
		case FTYPE_CYCLE:{
			char **cyclelist=(char **)fp->Widget->option;
			if(fp->Widget->value)
				for(n=0;cyclelist[n];n++)
					if(!stricmp(cyclelist[n],fp->Widget->value))
						break;
			if(cyclelist[n])
				set(fp->Widget->MUI_Object,MUIA_Cycle_Active,n);
			else
				set((Object *)fp->UserData,MUIA_Cycle_Active,0);
			break;}
		case FTYPE_LISTV:{
			char **cyclelist=(char **)fp->Widget->option;
			char **selectedlist=(char **)fp->Widget->value;
			long i;

			DoMethod((Object *)fp->UserData,MUIM_List_Select,MUIV_List_Select_All,MUIV_List_Select_Off,NULL);

			for(i=0;cyclelist[i];i++)
				for(n=0;selectedlist[n];n++)
					if(!stricmp(cyclelist[i],selectedlist[n]))
						DoMethod(fp->Widget->MUI_SubObject,MUIM_List_Select,i,MUIV_List_Select_On,NULL);

			break;}
		case FTYPE_CHECKBOX:
			for(n=0;((long **)fp->UserData)[n];n++){
				set((((WidgetInfo **)fp->UserData)[n])->MUI_SubObject,MUIA_Selected,(((WidgetInfo **)fp->UserData)[n])->checked);
				}
			break;
		case FTYPE_RADIO:
			for(n=0;((WidgetInfo **)fp->UserData)[n];n++){
				set((((WidgetInfo **)fp->UserData)[n])->MUI_SubObject,MUIA_Selected,(((WidgetInfo **)fp->UserData)[n])->checked);
				}
			break;
		}
}

static Object *ButtonPressed;

char *GetFormPartValue(FormPartInfo *fp,long *gadget)
{
	char *res = "";
	WidgetInfo *w;
	long i,t;
	BOOL found;

	switch(fp->Type){
		case FTYPE_BUTTON_S:
			if((fp->Widget->MUI_SubObject==ButtonPressed) && fp->Widget->name){
				res=fp->Widget->value;
				*gadget=-1;
				}
			else
				*gadget=-3;
			break;
		case FTYPE_TEXT:
		case FTYPE_PASSWORD:
			get((Object *)fp->UserData,MUIA_String_Contents,&res);
			*gadget=-1;
			break;
		case FTYPE_HIDDEN:
			res=((WidgetInfo *)fp->UserData)->value;
			*gadget=-1;
			break;
		case FTYPE_TEXTAREA:
			get((Object *)fp->UserData,MUIA_TEF_Text,&res);
			*gadget=-1;
			break;
		case FTYPE_CYCLE:
			get((Object *)fp->UserData,MUIA_Cycle_Active,&t);
			if(!(res=((char **)fp->Widget->returnv)[t]))
				res=((char **)fp->Widget->option)[t];
			*gadget=-1;
			break;
		case FTYPE_LISTV:
			if(*gadget==-2)
				t=MUIV_List_NextSelected_Start;
			else
				t=*gadget;

			DoMethod(fp->Widget->MUI_SubObject,MUIM_List_NextSelected,&t);
			if (t!=MUIV_List_NextSelected_End)
				if(!(res=((char **)fp->Widget->returnv)[t]))
					res=((char **)fp->Widget->option)[t];
			*gadget=t;
			DoMethod(fp->Widget->MUI_SubObject,MUIM_List_NextSelected,&t);
			if(t==MUIV_List_NextSelected_End)
				*gadget=-1;

			break;
		case FTYPE_CHECKBOX:
			found=FALSE;
			if(*gadget==-2)
				*gadget=0;
			for(;((WidgetInfo **)fp->UserData)[*gadget];(*gadget)++){
				w=((WidgetInfo **)fp->UserData)[*gadget];
				get(w->MUI_SubObject,MUIA_Selected,&t);
				if(t){
					if(!found){
						res=w->value;
						found=TRUE;
						}
					else
						break;
					}
				}
			if(!found)
				*gadget=-3; /* Mark end of list */
			break;
		case FTYPE_RADIO:
			*gadget=-3; /* No new value found */
			for(i=0;((WidgetInfo **)fp->UserData)[i];i++){
				w=((WidgetInfo **)fp->UserData)[i];
				get(w->MUI_SubObject,MUIA_Selected,&t);
				if(t){
					res=w->value;
					*gadget=-1; /* A value found */
					break;
					}
				}
			break;
		default:
			*gadget=-3; /* No new value found */
			break;
		}
	return res;
}

BOOL CatFormPart(char *buffer,FormPartInfo *fp,long *i)
{
	long gadgets=-2; /* -3="no new value" -2="initial value" -1="This was the last value" */
	char *value,*c;
	BOOL success=0;

	while(gadgets!=-1 && gadgets!=-3){
		if (fp->Name){
			value =GetFormPartValue(fp,&gadgets);
			if(gadgets!=-3){
				if(*i==-1 && (stricmp(fp->Name,"isindex")==0)){
					if(value){
						c = mo_escape_part (value);
						strcat (buffer, c);
						success=TRUE;
						free (c);
    					}
    				}
    			else{
					if(*i>0)
						strcat(buffer,"&");
					*i=1;
					c = mo_escape_part (fp->Name);
					strcat (buffer, c);
					success=TRUE;
					free (c);
	    
					strcat (buffer, "=");
					if(value){
						c = mo_escape_part (value);
						strcat (buffer, c);
						free (c);
	    				}
	    			}
				}
		}
		else
			gadgets=-1;
	}
	return success;
}

long GetFormPartLength(FormPartInfo *fp)
{
	long gadgets=-2,length=0;
	char *value;
	/* Rack 'em up. */

	while(gadgets!=-1 && gadgets!=-3){
		if (fp->Name){
			value =GetFormPartValue(fp,&gadgets);
			if(gadgets!=-3){
				if(gadgets!=0)
					length++;
				length+=strlen(fp->Name)+1;
				if(value){
					length+=strlen(value);
					}
    			}
		}
		else
			gadgets=-1;
	}
	return length;
}

struct FormButtonHookMsg
{
	WidgetInfo *widget;
	FormInfo *Form;
};

BOOL GetFormQuery(FormInfo *Form,char **res,char **resurl)
{
	int do_post_urlencoded = 0;
	char *query;
	long len, i;

	FormPartInfo *fp;
	char *url;
	char *method;
	BOOL success=0;


	/* Initial query: Breathing space. */
	len = 16;
	/* Add up lengths of strings. */
	for(fp=Form->FirstFormPart;fp;fp=fp->Next)
		len+=GetFormPartLength(fp)*3+2;

	/* Get the URL. */
	if (Form->href && *(Form->href))
		url = Form->href;
	else
		url = window.current_node->url;

	if (Form->method && *(Form->method))
		method = Form->method;
	else
		method = strdup ("GET");

	if (stricmp (method, "POST") == 0)
		do_post_urlencoded = 1;

	len += strlen (url);

	query = (char *)malloc (sizeof (char) * len);


	if (!do_post_urlencoded){
		strcpy (query, url);
		/* Clip out anchor. */
		strtok (query, "#");
		/* Clip out old query. */
		strtok (query, "?");
		if (query[strlen(query)-1] != '?'){
			strcat (query, "?");
			}
		}
	else query[0] = 0;

	for(fp=Form->FirstFormPart, i=0 ;fp;fp=fp->Next,i++)
		if(i==2) break; /* Check if it has more than one gadget */

	if(i>1) i=0; else i=-1;
	for(fp=Form->FirstFormPart;fp;fp=fp->Next)
		success|=CatFormPart(query,fp,&i);

	if(do_post_urlencoded) {
		*res=query;
		*resurl=url;
		}
	else {
		*res=NULL;
		*resurl=query;
		}

	return success;
}


void SubmitForm(struct FormButtonHookMsg *msg)
{
	char *query;

	char *url;

	GetFormQuery(msg->Form,&query,&url);

	if(url)
		if(!strnicmp("mailto:",url,7)){
#if DEBUG2==1
			kprintf("Mailto: isn't implemented YET\n");
#endif
			}
			
	if (query){
#if DEBUG2==1
		kprintf("URL: %s\nQuery: %s\n",url,query);
#endif
		SetAttrs(HTML_Gad,HTMLA_post_href,(IPTR)url,
						HTMLA_post_text,(IPTR)query,TAG_DONE);
		}
	else{
#if DEBUG2==1
		kprintf("URL: %s\n",url);
#endif
		set(HTML_Gad,HTMLA_get_href,(IPTR)url);
		}
}

HOOKPROTO(FormButtonFunc, IPTR, Object *object, struct FormButtonHookMsg *msg)
{
	FormPartInfo *fp;

	if(msg->widget->type==FTYPE_BUTTON_S){
		ButtonPressed=object;
		SubmitForm(msg);
		}
	else if(msg->widget->type==FTYPE_BUTTON_R){
		for(fp=msg->Form->FirstFormPart;fp;fp=fp->Next)
			ResetWidget(fp);
		}
	else if(msg->widget->type==FTYPE_BUTTON_C){
		for(fp=msg->Form->FirstFormPart;fp;fp=fp->Next)
			ClearWidget(fp);
		}
	return 0;
}

HOOKPROTONO(FormStringFunc, IPTR, struct FormButtonHookMsg *msg)
{
	SubmitForm(msg);
	return 0;
}

MakeHook(FormButton_hook, FormButtonFunc);
MakeHook(FormString_hook, FormStringFunc);

void ImageSubmitForm(FormInfo *fptr,char *name,int x,int y)
{
	char *query,*newquery,*url;
	long len;
	long res;
	char valstr[10];
	res=GetFormQuery(fptr,&query,&url);

	if(query){
		len=strlen(query)+(name ? 2*strlen(name) : 0)+16; /* Coordinates larger than 6 diggits is _highly_ unlikely */

		newquery=malloc(len);
		strcpy(newquery,query);
		free(query);

		if(res)
			strcat(newquery,"&");
		if(name){
			strcat(newquery,name);
			strcat(newquery,".x=");
			}
		else
			strcat(newquery,"x=");
		sprintf(valstr, "%d&", x);
		strcat(newquery,valstr);

		if(name){
			strcat(newquery,name);
			strcat(newquery,".y=");
			}
		else
			strcat(newquery,"y=");
		sprintf(valstr, "%d", y);
		strcat(newquery,valstr);

#if DEBUG2==1
		kprintf("Query %s\n",newquery);
#endif

		SetAttrs(HTML_Gad,HTMLA_post_href,(IPTR)url,
					HTMLA_post_text,(IPTR)newquery,TAG_DONE);
		}
	else{
		len=strlen(url)+(name ? 2*strlen(name) : 0)+16; /* Coordinates larger than 6 diggits is _highly_ unlikely */

		newquery=malloc(len);
		strcpy(newquery,url);
		free(url);

		if(res)
			strcat(newquery,"&");
		else
			strcat(newquery,"?");

		if(name){
			strcat(newquery,name);
			strcat(newquery,".x=");
			}
		else
			strcat(newquery,"x=");
		sprintf(valstr, "%d&", x);
		strcat(newquery,valstr);

		if(name){
			strcat(newquery,name);
			strcat(newquery,".y=");
			}
		else
			strcat(newquery,"y=");
		sprintf(valstr, "%d", y);
		strcat(newquery,valstr);

#if DEBUG2==1
		kprintf("Query %s\n",newquery);
#endif

		set(HTML_Gad,HTMLA_get_href,(IPTR)newquery);
		}
}


HOOKPROTONO(FormRadioFunc, IPTR, struct FormButtonHookMsg *msg)
{
	WidgetInfo *cw;

	for(cw=msg->widget->PrevRadio;cw;cw=cw->PrevRadio)
		set(cw->MUI_SubObject,MUIA_Selected,FALSE);
	for(cw=msg->widget->NextRadio;cw;cw=cw->NextRadio)
		set(cw->MUI_SubObject,MUIA_Selected,FALSE);
	return 0;
}

MakeHook(FormRadio_hook, FormRadioFunc);

char *ComposeCommaList(char **values, int nof){
	long n,size=0,oldvalue;
	char **vt=values,*str;
	oldvalue=0;
	for(n=nof;n;n--){
		if(oldvalue) size++;
		if(*vt) size+=strlen(*vt++);
		else vt++;
		oldvalue=1;
		}
	str=malloc(size+1);
	*str=0x0;
	oldvalue=0;
	for(n=nof;n;n--){
		if(oldvalue) strcat(str,"|");
		if(*values) strcat(str,*values++);
		else values++;
		oldvalue=1;
		}
	return str;
}

struct TextEditHookMsg
{
	FormInfo *Form;
};

HOOKPROTO(TextEditFunc, IPTR, Object *obj, struct TextEditHookMsg *msg)
{
	LONG res = 0;

	get(obj,MUIA_TEF_Active,&res);

	if(res)
		set(HTML_Gad,HTMLA_ScrollKeys,FALSE);
	else
		set(HTML_Gad,HTMLA_ScrollKeys,TRUE);
	return 0;
}

MakeHook(TextEdit_hook, TextEditFunc);

void FreeCommaList(char **values, int nof)
{
	for(;nof;nof--)
		if(*values)
			free(*values++);
}

/*
** Custom layout function.
** Perform several actions according to the messages lm_Type
** field. Note that you must return MUILM_UNKNOWN if you do
** not implement a specific lm_Type.
*/

HOOKPROTONO(LayoutFunc, IPTR, struct MUI_LayoutMsg *lm)
{
	HTMLGadClData *inst=INST_DATA(HTMLGadClass,HTML_Gad);
	switch (lm->lm_Type)
	{

		case MUILM_MINMAX:
		{
#if 0
			/*
			** MinMax calculation function. When this is called,
			** the children of your group have already been asked
			** about their min/max dimension so you can use their
			** dimensions to calculate yours.
			**
			** In this example, we make our minimum size twice as
			** big as the biggest child in our group.
			*/

			Object *cstate = (Object *)lm->lm_Children->mlh_Head;
			Object *child;

			WORD maxminwidth  = 0;
			WORD maxminheight = 0;

			/* find out biggest widths & heights of our children */

			while (child = NextObject(&cstate))
			{
				if (maxminwidth <MUI_MAXMAX && _minwidth (child) > maxminwidth ) maxminwidth  = _minwidth (child);
				if (maxminheight<MUI_MAXMAX && _minheight(child) > maxminheight) maxminheight = _minheight(child);
			}

			/* set the result fields in the message */
#endif
			lm->lm_MinMax.MinWidth  = 5000; /* Don't care since this */
			lm->lm_MinMax.MinHeight = 5000; /* is a virtual group */
			lm->lm_MinMax.DefWidth  = 5000;
			lm->lm_MinMax.DefHeight = 5000;
			lm->lm_MinMax.MaxWidth  = 5000;
			lm->lm_MinMax.MaxHeight = 5000;

			return(0);
		}

		case MUILM_LAYOUT:
		{
			Object *cstate,*child;

			/*
			** Layout function. Here, we have to call MUI_Layout() for each
			** our children. MUI wants us to place them in a rectangle
			** defined by (0,0,lm->lm_Layout.Width-1,lm->lm_Layout.Height-1)
			** You are free to put the children anywhere in this rectangle.
			**
			** If you are a virtual group, you may also extend
			** the given dimensions and place your children anywhere. Be sure
			** to return the dimensions you need in lm->lm_Layout.Width and
			** lm->lm_Layout.Height in this case.
			**
			** Return TRUE if everything went ok, FALSE on error.
			** Note: Errors during layout are not easy to handle for MUI.
			**       Better avoid them!
			*/


			/* Reformat the text */

			inst->view_left = _mleft(HTML_Gad);
			inst->view_top = _mtop(HTML_Gad);
			inst->view_width = _mwidth(HTML_Gad);
			inst->view_height = _mheight(HTML_Gad);
			inst->rp=_rp(HTML_Gad);
			inst->win=_window(HTML_Gad);


			inst->object_flags|=Object_Reparsing;
			ReformatWindow(inst);
			inst->object_flags&=~Object_Reparsing;

			/* Place the objects */

			cstate = (Object *)lm->lm_Children->mlh_Head;

			while ((child = NextObject(&cstate)))
			{
				if(child==G_Forms){ /* Place the main object upper left */
					if(!MUI_Layout(child,0,0,_defwidth(child),_defwidth(child),0))
						return(FALSE);
					}
				else
					DoMethod(child,MUIM_NewGroup_Layout);
			}

			lm->lm_Layout.Width  = inst->doc_width;
			lm->lm_Layout.Height = inst->doc_height;

			return(TRUE);
		}
	}
	return(MUILM_UNKNOWN);
}

MakeHook(LayoutHook, LayoutFunc);

#if 0
Calling sequence when loading new page:

main.c calls gui-documents.c/mo_load_window_text (see below)
end

mo_load_window_text:
Calls: gui-documents.c/mo_do_window_text (see below) /* Hide, Parse and Reformat */
Calls: HTMLamiga.c/LoadInlinedImages
Calls: HTMLamiga.c/ResetAmigaGadgets
Redraws the HTML_Gad
end

mo_do_window_text:
Calls gui-documents.c/mo_set_text (see below)
end

mo_set_text:
Calls HTML.c/HTMLSetText (see below)
end

HTMLSetText
Calls HTMLamiga.c/HideWidgets
Calls HTMLamiga.c/HTMLParse
Calls HTML.c/ReformatWindow
end

mo_back_node:
Calls mo_set_win_current_node



Check this URL!
http://www.ncsa.uiuc.edu/SDG/Software/Mosaic/Docs/fill-out-forms/overview.html

http://www.phone.net/cgi-bin/index/ /* Mike Meyer home page */

http://web2.xerox.com/digitrad /* Isindex */

http://www.pdi.lodz.pl/~robert /* Escom news */

test case:
1:	Ok
2:	Ok
3:	Ok
4:	Ok
5:	Ok
6:	Ok
7:	Ok
8:	Ok
9:	Ok
10:	Ok
11:	Ok
12:	Ok
13:	Ok

All in all:

Editing does not work.

#endif

struct MUI_CustomClass *HTMLGadClInit(void)
{
	struct MUI_CustomClass *cl;

	/* Create the HTML gadget class */
	if (!(cl = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,
						sizeof(HTMLGadClData),ENTRY(HTMLGadClDispatch))))
		fail(NULL, "Failed to create HTMLGad class."); /* Failed, cleanup */

	if (!(HTMLText = MUI_CreateCustomClass(NULL,MUIC_Area,NULL,
						sizeof(HTMLTextClData),ENTRY(HTMLTextClDispatch))))
		fail(NULL, "Failed to create HTMLText class."); /* Failed, cleanup */

	if (!(HTMLTextN = MUI_CreateCustomClass(NULL,MUIC_Area,NULL,
						sizeof(HTMLTextNClData),ENTRY(HTMLTextNClDispatch))))
		fail(NULL, "Failed to create HTMLTextN class."); /* Failed, cleanup */

  return cl;
}

/*******************************************************
  Free our MUI HTML Gadget Class
*******************************************************/
BOOL HTMLGadClFree(struct MUI_CustomClass *mcc)
{
	MUI_DeleteCustomClass(HTMLText);
	MUI_DeleteCustomClass(HTMLTextN);
	MUI_DeleteCustomClass(mcc);
	return 0;
}

