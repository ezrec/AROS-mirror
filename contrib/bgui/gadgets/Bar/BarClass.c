/*
 * @(#) $Header$
 *
 * BGUI library
 *
 * (C) Copyright 2000 BGUI Developers Team.
 * (C) Copyright 1998 Manuel Lemos.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.8  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.7  2003/01/18 19:10:13  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.6  2000/08/11 08:09:38  chodorowski
 * Removed METHOD #define, already defined in bgui_compilerspecific.h.
 *
 * Revision 42.5  2000/08/09 10:17:24  chodorowski
 * #include <bgui/bgui_compilerspecific.h> for the REGFUNC and REGPARAM
 * macros. Some of these files didn't need them at all...
 *
 * Revision 42.4  2000/08/08 13:51:34  chodorowski
 * Removed all REGFUNC, REGPARAM and REG macros. Now includes
 * contrib/bgui/compilerspecific.h where they are defined.
 *
 * Revision 42.3  2000/07/11 17:05:14  stegerg
 * BGUIBase is struct Library *, not struct BGUIBase.
 *
 * Revision 42.2  2000/07/09 03:05:08  bergers
 * Makes the gadgets compilable.
 *
 * Revision 42.1  2000/05/15 19:29:07  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:20:29  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:34:19  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:24  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  2000/05/04 05:07:58  mlemos
 * Initial revision.
 *
 *
 */


#ifdef __AROS__
extern struct Library * BGUIBase;
#endif


void kprintf(char *format,...);
#define bug kprintf

#ifdef DEBUG_BAR_CLASS
#define D(x) (x)
#else
#define D(x) ;
#endif

#define DB(x) (x)

#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <clib/alib_protos.h>
#include <proto/bgui.h>

#include <bgui/bgui_bar.h>
#include "include/lGraphic.h"

#ifndef BGUI_COMPILERSPECIFIC_H
#include <bgui/bgui_compilerspecific.h>
#endif

#define PointInBox(box,x,y) ((x)>=(box).Left && (x)<(box).Left+(box).Width && (y)>=(box).Top && (y)<(box).Top+(box).Height)

typedef struct
{
	BOOL Horizontal;
	BOOL Vertical;
	ULONG HorizontalPosition;
	ULONG HorizontalSize;
	ULONG HorizontalTotal;
	ULONG HorizontalPage;
	ULONG VerticalPosition;
	ULONG VerticalSize;
	ULONG VerticalTotal;
	ULONG VerticalPage;
	ULONG MinimumKnobWidth;
	ULONG MinimumKnobHeight;
	BOOL Disabled;

	Object *KnobImage;
	ULONG KnobImageFrameWidth;
	ULONG KnobImageFrameHeight;
	struct IBox HitBox,KnobBox,LastUpdate,LastUpdateHitBox;
	LONG Left,Top;
	BOOL Holding;
	LONG HoldingLeft,HoldingTop;
	ULONG StartHorizontalPosition;
	ULONG StartVerticalPosition;
}
GD;

static ULONG instances=0;

static void ComputeHitBox(Object *obj,GD *gd)
{
	struct IBox *hit_box;
	Object *frame;
	ULONG frame_width,frame_height;

	if(DoMethod(obj,OM_GET,BT_HitBox,&hit_box)
	&& hit_box->Width
	&& hit_box->Height
	&& DoMethod(obj,OM_GET,BT_FrameObject,&frame)
	&& DoMethod(frame,OM_GET,FRM_FrameWidth,&frame_width)
	&& DoMethod(frame,OM_GET,FRM_FrameHeight,&frame_height)
	&& hit_box->Width>2*frame_width
	&& hit_box->Height>2*frame_width)
	{
		gd->HitBox.Left=(gd->Left=hit_box->Left)+frame_width;
		gd->HitBox.Width=hit_box->Width-2*frame_width;
		gd->HitBox.Top=(gd->Top=hit_box->Top)+frame_height;
		gd->HitBox.Height=hit_box->Height-2*frame_height;
	}
	else
	{
		gd->HitBox.Left=
		gd->HitBox.Top=
		gd->HitBox.Width=
		gd->HitBox.Height=0;
	}
}

static void ComputeBarBox(Object *obj,GD *gd)
{
	if(gd->HitBox.Width
	&& gd->HitBox.Height)
	{
		if(gd->HorizontalSize
		&& gd->VerticalSize)
		{
			ULONG size;

			D(bug("Horizontal Position-%lu, Size-%lu, Total %lu\n",gd->HorizontalPosition,gd->HorizontalSize,gd->HorizontalTotal));
			size=(gd->HorizontalPosition+gd->HorizontalSize<=gd->HorizontalTotal ? gd->HorizontalSize : gd->HorizontalTotal-gd->HorizontalSize);
			if((gd->KnobBox.Width=((double)gd->HitBox.Width*(double)size/(double)gd->HorizontalTotal))<gd->MinimumKnobWidth+2*gd->KnobImageFrameWidth)
				gd->KnobBox.Width=gd->MinimumKnobWidth+2*gd->KnobImageFrameWidth;
			gd->KnobBox.Left=gd->HitBox.Left;
			if(gd->HorizontalPosition
			&& gd->HorizontalTotal-gd->HorizontalSize>0)
				gd->KnobBox.Left+=((double)(gd->HitBox.Width-gd->KnobBox.Width)*(double)gd->HorizontalPosition/(double)(gd->HorizontalTotal-gd->HorizontalSize));
			D(bug("Vertical Position-%lu, Size-%lu, Total %lu\n",gd->VerticalPosition,gd->VerticalSize,gd->VerticalTotal));
			size=(gd->VerticalPosition+gd->VerticalSize<=gd->VerticalTotal ? gd->VerticalSize : gd->VerticalTotal-gd->VerticalSize);
			if((gd->KnobBox.Height=((double)gd->HitBox.Height*(double)size/(double)gd->VerticalTotal))<gd->MinimumKnobHeight+2*gd->KnobImageFrameHeight)
				gd->KnobBox.Height=gd->MinimumKnobHeight+2*gd->KnobImageFrameHeight;
			gd->KnobBox.Top=gd->HitBox.Top;
			if(gd->VerticalPosition
			&& gd->VerticalTotal-gd->VerticalSize>0)
				gd->KnobBox.Top+=((double)(gd->HitBox.Height-gd->KnobBox.Height)*(double)gd->VerticalPosition/(double)(gd->VerticalTotal-gd->VerticalSize));
		}
		else
		{
			gd->KnobBox.Left=
			gd->KnobBox.Top=
			gd->KnobBox.Width=
			gd->KnobBox.Height=0;
		}
		D(bug("Knob Box Left %ld, Top %ld, Width %lu, Height %lu\n",gd->KnobBox.Left,gd->KnobBox.Top,gd->KnobBox.Width,gd->KnobBox.Height));
	}
	else
	{
		gd->Left=
		gd->Top=
		gd->HitBox.Left=
		gd->HitBox.Top=
		gd->HitBox.Width=
		gd->HitBox.Height=
		gd->KnobBox.Left=
		gd->KnobBox.Top=
		gd->KnobBox.Width=
		gd->KnobBox.Height=0;
	}
	D(bug("Hit Box Left %ld, Top %ld, Width %lu, Height %lu\n",gd->HitBox.Left,gd->HitBox.Top,gd->HitBox.Width,gd->HitBox.Height));
}

static void DrawBar(Object *obj,GD *gd,struct RastPort *rastport,struct GadgetInfo *gadget_info,BOOL recompute,BOOL update)
{
	ComputeHitBox(obj,gd);
	if(recompute
	|| gd->LastUpdateHitBox.Width!=gd->HitBox.Width
	|| gd->LastUpdateHitBox.Height!=gd->HitBox.Height)
	{
		ComputeBarBox(obj,gd);
		update=FALSE;
	}
	if(gd->HitBox.Width
	&& gd->HitBox.Height)
	{
		if(GfxBase->LibNode.lib_Version<39)
		{
			SetAPen(rastport,(ULONG)gadget_info->gi_DrInfo->dri_Pens[BACKGROUNDPEN]);
			SetDrMd(rastport,JAM1);
		}
		else
			SetABPenDrMd(rastport,(ULONG)gadget_info->gi_DrInfo->dri_Pens[BACKGROUNDPEN],0,JAM1);
		SetAfPt(rastport,NULL,0);
		if(update)
		{
			if(gd->LastUpdate.Width
			&& gd->LastUpdate.Height)
			{
				liRECTANGLE last,current,subtraction[4];
				lgCARDINAL subtracted;

				last.Origin.X=gd->LastUpdate.Left;
				last.Dimension.Width=gd->LastUpdate.Width;
				last.Origin.Y=gd->LastUpdate.Top;
				last.Dimension.Height=gd->LastUpdate.Height;
				current.Origin.X=gd->KnobBox.Left;
				current.Dimension.Width=gd->KnobBox.Width;
				current.Origin.Y=gd->KnobBox.Top;
				current.Dimension.Height=gd->KnobBox.Height;
				if((subtracted=liSubtractRectangles(&last,&current,subtraction)))
				{
					liRECTANGLE *rectangle;
	
					for(rectangle= &subtraction[subtracted-1];subtracted;rectangle--,subtracted--)
						RectFill(rastport,
							rectangle->Origin.X,
							rectangle->Origin.Y,
							rectangle->Origin.X+rectangle->Dimension.Width-1,
							rectangle->Origin.Y+rectangle->Dimension.Height-1);
				}
			}
		}
		else
		{
			if(gd->HitBox.Width
			&& gd->HitBox.Height)
				RectFill(rastport,(LONG)gd->HitBox.Left,(LONG)gd->HitBox.Top,(LONG)(gd->HitBox.Left+gd->HitBox.Width-1),(LONG)(gd->HitBox.Top+gd->HitBox.Height-1));
		}
		if(gd->KnobBox.Width
		&& gd->KnobBox.Height)
		{
			ULONG pen,frame_type,normal_frame_type;
	
			if(gd->Horizontal
			|| gd->Vertical)
			{
				normal_frame_type=FRTYPE_BUTTON;
				if(gd->KnobBox.Width>2*gd->KnobImageFrameWidth
				&& gd->KnobBox.Height>2*gd->KnobImageFrameHeight)
				{
					pen=(gd->Holding ? FILLPEN : BACKGROUNDPEN);
					frame_type=FRTYPE_BUTTON;
				}
				else
				{
					pen=BLOCKPEN;
					frame_type=FRTYPE_NONE;
				}
			}
			else
			{
				pen=FILLPEN;
				normal_frame_type=frame_type=FRTYPE_NONE;
			}
			SetAttrs(gd->KnobImage,
				IA_Left,gd->KnobBox.Left,
				IA_Top,gd->KnobBox.Top,
				IA_Width,gd->KnobBox.Width,
				IA_Height,gd->KnobBox.Height,
				FRM_BackDriPen,pen,
				FRM_Type,frame_type,
				TAG_END);
			DrawImageState(rastport,(struct Image *)gd->KnobImage,0,0,gd->Holding ? IDS_SELECTED : IDS_NORMAL,gadget_info->gi_DrInfo);
			if(normal_frame_type!=frame_type)
			{
				SetAttrs(gd->KnobImage,
					FRM_Type,normal_frame_type,
					TAG_END);
			}
		}
		if((gd->Horizontal
		|| gd->Vertical)
		&& ((struct Gadget *)obj)->Flags & GFLG_DISABLED)
		{
			static UWORD disabled_pattern[2]={0x2222,0x8888};

			if(GfxBase->LibNode.lib_Version<39)
			{
				SetAPen(rastport,(ULONG)gadget_info->gi_DrInfo->dri_Pens[SHADOWPEN]);
				SetDrMd(rastport,JAM1);
			}
			else
				SetABPenDrMd(rastport,(ULONG)gadget_info->gi_DrInfo->dri_Pens[SHADOWPEN],0,JAM1);
			SetAfPt(rastport,disabled_pattern,1);
			RectFill(rastport,(LONG)gd->HitBox.Left,(LONG)gd->HitBox.Top,(LONG)(gd->HitBox.Left+gd->HitBox.Width-1),(LONG)(gd->HitBox.Top+gd->HitBox.Height-1));
		}
		gd->LastUpdate=gd->KnobBox;
		gd->LastUpdateHitBox=gd->HitBox;
	}
}

static void DrawGadget(Object *obj,GD *gd,struct GadgetInfo *gadget_info,BOOL release,BOOL recompute,BOOL update)
{
	struct RastPort *rastport;

	if(release)
	{
		if(gd->Holding)
			gd->Holding=FALSE;
		else
			return;
	}
	if(recompute
	&& update)
	{
		ComputeHitBox(obj,gd);
		ComputeBarBox(obj,gd);
	}
	if((rastport=ObtainGIRPort(gadget_info)))
	{
		DoMethod(obj,GM_RENDER,gadget_info,rastport,update ? GREDRAW_UPDATE : GREDRAW_REDRAW);
		ReleaseGIRPort(rastport);
	}
}

static void NotifyValue(
Object *obj,
GD *gd,
struct GadgetInfo *gadget_info,
ULONG flags,
ULONG attribute,
ULONG value)
{
	struct TagItem tags[3];

	tags[0].ti_Tag=GA_ID;
	tags[0].ti_Data=((struct Gadget *)obj)->GadgetID;
	tags[1].ti_Tag=attribute;
	tags[1].ti_Data=value;
	tags[2].ti_Tag=TAG_END;
	DoMethod(obj,OM_NOTIFY,tags,gadget_info,flags);
}

static void NotifyKnob(
Object *obj,
GD *gd,
struct GadgetInfo *gadget_info,
ULONG flags,
BOOL notify_horizontal_position,
BOOL notify_vertical_position)
{
	D(bug("Notify ID %lu, %sHorizontal %lu, %sVertical %lu\n",((struct Gadget *)obj)->GadgetID,notify_horizontal_position? "+" : "",gd->HorizontalPosition,notify_vertical_position? "+" : "",gd->VerticalPosition));
	if(notify_horizontal_position)
		NotifyValue(obj,gd,gadget_info,flags,BAR_HorizontalPosition,gd->HorizontalPosition);
	if(notify_vertical_position)
		NotifyValue(obj,gd,gadget_info,flags,BAR_VerticalPosition,gd->VerticalPosition);
}

static BOOL UpdateKnob(
Object *obj,
GD *gd,
struct GadgetInfo *gadget_info,
ULONG flags,
ULONG horizontal_position,
ULONG vertical_position,
BOOL force_redraw,
BOOL recompute)
{
	if(horizontal_position>gd->HorizontalTotal-gd->HorizontalSize)
		horizontal_position=gd->HorizontalTotal-gd->HorizontalSize;
	if(vertical_position>gd->VerticalTotal-gd->VerticalSize)
		vertical_position=gd->VerticalTotal-gd->VerticalSize;
	if(horizontal_position!=gd->HorizontalPosition
	|| vertical_position!=gd->VerticalPosition)
	{
		BOOL notify_horizontal_position,notify_vertical_position;

		notify_horizontal_position=(horizontal_position!=gd->HorizontalPosition);
		notify_vertical_position=(vertical_position!=gd->VerticalPosition);
		gd->HorizontalPosition=horizontal_position;
		gd->VerticalPosition=vertical_position;
		DrawGadget(obj,gd,gadget_info,FALSE,recompute,!force_redraw);
		NotifyKnob(obj,gd,gadget_info,flags,notify_horizontal_position,notify_vertical_position);
		return(TRUE);
	}
	else
	{
		if(force_redraw)
			DrawGadget(obj,gd,gadget_info,FALSE,recompute,FALSE);
	}
	return(FALSE);
}

#define SET_FAIL    0
#define SET_SUCCESS 1
#define SET_UNKNOWN 2

static ULONG BarSet(Class *cl,Object *obj,struct opUpdate *opu,BOOL creation)
{
	GD *gd;
	struct TagItem *tag,*tstate;
	ULONG success;
	BOOL redraw,redraw_knob,position_changed;
	LONG horizontal_position,vertical_position;
	ULONG horizontal_size,vertical_size,horizontal_total,vertical_total,horizontal_page,vertical_page;
	ULONG flags;

	D(bug("BarSet\n"));
	gd=INST_DATA(cl,obj);
	horizontal_position=gd->HorizontalPosition;
	vertical_position=gd->VerticalPosition;
	horizontal_size=gd->HorizontalSize;
	vertical_size=gd->VerticalSize;
	horizontal_total=gd->HorizontalTotal;
	vertical_total=gd->VerticalTotal;
	horizontal_page=gd->HorizontalPage;
	vertical_page=gd->VerticalPage;
	for(redraw_knob=FALSE,redraw=FALSE,tstate=opu->opu_AttrList,success=SET_UNKNOWN;(tag=NextTagItem(&tstate));)
	{
		D(bug("Tag %lx, Value %lu\n",tag->ti_Tag,tag->ti_Data));
		switch(tag->ti_Tag)
		{
			case BAR_Horizontal:
				if(creation==FALSE)
					return(SET_FAIL);
				gd->Horizontal=(tag->ti_Data!=0);
				break;
			case BAR_Vertical:
				if(creation==FALSE)
					return(SET_FAIL);
				gd->Vertical=(tag->ti_Data!=0);
				break;
			case BAR_HorizontalPosition:
				horizontal_position=tag->ti_Data;
				break;
			case BAR_HorizontalSize:
				if(gd->HorizontalSize!=tag->ti_Data)
				{
					gd->HorizontalSize=tag->ti_Data;
					redraw_knob=TRUE;
				}
				break;
			case BAR_HorizontalTotal:
				if(tag->ti_Data==0)
					return(SET_FAIL);
				if(gd->HorizontalTotal!=tag->ti_Data)
				{
					gd->HorizontalTotal=tag->ti_Data;
					redraw_knob=TRUE;
				}
				break;
			case BAR_HorizontalPage:
				gd->HorizontalPage=tag->ti_Data;
				break;
			case BAR_VerticalPosition:
				vertical_position=tag->ti_Data;
				break;
			case BAR_VerticalSize:
				if(gd->VerticalSize!=tag->ti_Data)
				{
					gd->VerticalSize=tag->ti_Data;
					redraw_knob=TRUE;
				}
				break;
			case BAR_VerticalTotal:
				if(tag->ti_Data==0)
					return(SET_FAIL);
				if(gd->VerticalTotal!=tag->ti_Data)
				{
					gd->VerticalTotal=tag->ti_Data;
					redraw_knob=TRUE;
				}
				break;
			case BAR_VerticalPage:
				gd->VerticalPage=tag->ti_Data;
				break;
			case BAR_MinimumKnobWidth:
				if(tag->ti_Data==0)
					return(SET_FAIL);
				if(gd->MinimumKnobWidth!=tag->ti_Data)
				{
					gd->MinimumKnobWidth=tag->ti_Data;
					redraw_knob=TRUE;
				}
				break;
			case BAR_MinimumKnobHeight:
				if(tag->ti_Data==0)
					return(SET_FAIL);
				if(gd->MinimumKnobHeight!=tag->ti_Data)
				{
					gd->MinimumKnobHeight=tag->ti_Data;
					redraw_knob=TRUE;
				}
				break;
			case GA_Disabled:
				if(gd->Disabled!=(tag->ti_Data!=0))
				{
					gd->Disabled=(tag->ti_Data!=0);
					if(gd->Horizontal
					|| gd->Vertical)
						redraw=TRUE;
				}
				break;
			default:
				continue;
		}
		success=SET_SUCCESS;
	}
	flags=(ULONG)(opu->MethodID==OM_UPDATE ? opu->opu_Flags : 0);
	position_changed=(horizontal_position!=gd->HorizontalPosition || vertical_position!=gd->VerticalPosition);
	if(!creation
	&& (redraw
	|| redraw_knob
	|| position_changed))
	{
		if(UpdateKnob(obj,gd,opu->opu_GInfo,flags,horizontal_position,vertical_position,redraw,position_changed || opu->MethodID!=OM_UPDATE)==FALSE
		&& (redraw_knob
		|| (!gd->Holding
		&& position_changed))
		&& !redraw)
			DrawGadget(obj,gd,opu->opu_GInfo,FALSE,TRUE,TRUE);
	}
	else
	{
		gd->HorizontalPosition=horizontal_position;
		gd->VerticalPosition=vertical_position;
	}
	if(horizontal_position!=gd->HorizontalPosition)
		NotifyValue(obj,gd,opu->opu_GInfo,flags,BAR_HorizontalPosition,gd->HorizontalPosition);
	if(vertical_position!=gd->VerticalPosition)
		NotifyValue(obj,gd,opu->opu_GInfo,flags,BAR_VerticalPosition,gd->VerticalPosition);
	if(horizontal_size!=gd->HorizontalSize)
		NotifyValue(obj,gd,opu->opu_GInfo,flags,BAR_HorizontalSize,gd->HorizontalSize);
	if(vertical_size!=gd->VerticalSize)
		NotifyValue(obj,gd,opu->opu_GInfo,flags,BAR_VerticalSize,gd->VerticalSize);
	if(horizontal_total!=gd->HorizontalTotal)
		NotifyValue(obj,gd,opu->opu_GInfo,flags,BAR_HorizontalTotal,gd->HorizontalTotal);
	if(vertical_total!=gd->VerticalTotal)
		NotifyValue(obj,gd,opu->opu_GInfo,flags,BAR_VerticalTotal,gd->VerticalTotal);
	if(horizontal_page!=gd->HorizontalPage)
		NotifyValue(obj,gd,opu->opu_GInfo,flags,BAR_HorizontalPage,gd->HorizontalPage);
	if(vertical_page!=gd->VerticalPage)
		NotifyValue(obj,gd,opu->opu_GInfo,flags,BAR_VerticalPage,gd->VerticalPage);
	D(bug("BarSet %lu\n",success));
	return(success);
}

METHOD(BarClassNew,struct opSet *,ops)
{
	D(bug("BarClassNew\n"));
	if(BGUIBase==NULL
	&& (BGUIBase=OpenLibrary("bgui.library",41))==NULL)
		return(NULL);
	{
		struct TagItem default_tags[3];

		default_tags[0].ti_Tag=FRM_Type;
		default_tags[0].ti_Data=FRTYPE_DROPBOX;
		default_tags[1].ti_Tag=FRM_Recessed;
		default_tags[1].ti_Data=FALSE;
		default_tags[2].ti_Tag=TAG_MORE;
		default_tags[2].ti_Data=(ULONG)ops->ops_AttrList;
		ops->ops_AttrList=default_tags;
		obj=(Object *)DoSuperMethodA(cl,obj,(Msg)ops);
		ops->ops_AttrList=(struct TagItem *)default_tags[2].ti_Data;
	}
	if(obj)
	{
		GD *gd;
		Object *frame;
		ULONG thin_frame=TRUE;

		instances++;
		gd=INST_DATA(cl,obj);
		gd->KnobImage=NULL;
		gd->Horizontal=
		gd->Vertical=FALSE;
		gd->HorizontalPosition=
		gd->VerticalPosition=0;
		gd->HorizontalSize=
		gd->HorizontalTotal=
		gd->HorizontalPage=
		gd->VerticalSize=
		gd->VerticalTotal=
		gd->VerticalPage=1;
		gd->MinimumKnobWidth=KNOBHMIN;
		gd->MinimumKnobHeight=KNOBVMIN;
		gd->Disabled= !(((struct Gadget *)obj)->Flags & GFLG_DISABLED);
		gd->Holding=FALSE;
		gd->LastUpdate.Left=
		gd->LastUpdate.Top=
		gd->LastUpdate.Width=
		gd->LastUpdate.Height=0;
		gd->LastUpdateHitBox.Left=
		gd->LastUpdateHitBox.Top=
		gd->LastUpdateHitBox.Width=
		gd->LastUpdateHitBox.Height=0;
		if(DoMethod(obj,OM_GET,BT_FrameObject,&frame)==0
		|| DoMethod(frame,OM_GET,FRM_ThinFrame,&thin_frame)==0
		|| BarSet(cl,obj,(struct opUpdate *)ops,TRUE)==SET_FAIL
		|| (gd->KnobImage=BGUI_NewObject(BGUI_FRAME_IMAGE,
			FRM_Type,(gd->Horizontal || gd->Vertical) ? FRTYPE_BUTTON : FRTYPE_NONE,
			FRM_ThinFrame,thin_frame,
			TAG_END))==NULL
		|| DoMethod(gd->KnobImage,OM_GET,FRM_FrameWidth,&gd->KnobImageFrameWidth)==0
		|| DoMethod(gd->KnobImage,OM_GET,FRM_FrameHeight,&gd->KnobImageFrameHeight)==0)
		{
			CoerceMethod(cl,obj,OM_DISPOSE);
			obj=NULL;
		}
	}
	if(instances==0
	&& BGUIBase)
	{
		CloseLibrary(BGUIBase);
		BGUIBase=NULL;
	}
	return((ULONG)obj);
}
METHOD_END

METHOD(BarClassDispose,Msg,msg)
{
	GD *gd=INST_DATA(cl,obj);
	ULONG rc;

	D(bug("BarClassDispose\n"));
	if(gd->KnobImage)
		DoMethod(gd->KnobImage,OM_DISPOSE);
	rc=DoSuperMethodA(cl,obj,msg);
	if(--instances==0)
	{
		CloseLibrary(BGUIBase);
		BGUIBase=NULL;
	}
	return(rc);
}
METHOD_END

METHOD(BarClassSet,struct opUpdate *,opu)
{
	BOOL success;

	D(bug("BarClassSet\n"));
	success=DoSuperMethodA(cl,obj,(Msg)opu);
	switch(BarSet(cl,obj,opu,FALSE))
	{
		case SET_FAIL:
			success=FALSE;
			break;
		case SET_SUCCESS:
			success=TRUE;
			break;
	}
	D(bug("BarClassSet success %ld\n",success));
	return(success);
}
METHOD_END

static GetAttribute(Class *cl,GD *gd,ULONG attribute,ULONG *store)
{
	switch(attribute)
	{
		case BAR_Horizontal:
			*store=gd->Horizontal;
			break;
		case BAR_Vertical:
			*store=gd->Vertical;
			break;
		case BAR_HorizontalPosition:
			*store=gd->HorizontalPosition;
			break;
		case BAR_HorizontalSize:
			*store=gd->HorizontalSize;
			break;
		case BAR_HorizontalTotal:
			*store=gd->HorizontalTotal;
			break;
		case BAR_VerticalPosition:
			*store=gd->VerticalPosition;
			break;
		case BAR_VerticalSize:
			*store=gd->VerticalSize;
			break;
		case BAR_VerticalTotal:
			*store=gd->VerticalTotal;
			break;
   case BT_PostRenderHighestClass:
      *store=(ULONG)cl;
      break;
		default:
			return(FALSE);
	}
	return(TRUE);
}

METHOD(BarClassGet,struct opGet *,opg)
{
	D(bug("BarClassGet\n"));
	return(GetAttribute(cl,INST_DATA(cl,obj),opg->opg_AttrID,opg->opg_Storage) ? 1 : DoSuperMethodA(cl,obj,(Msg)opg));
}
METHOD_END

METHOD(BarClassRender, struct gpRender *,gpr)
{
	GD *gd=INST_DATA(cl,obj);
	ULONG result;

	D(bug("BarClassRender\n"));
	if((result=DoSuperMethodA(cl,obj,(Msg)gpr)))
		DrawBar(obj,gd,gpr->gpr_RPort,gpr->gpr_GInfo,gpr->gpr_Redraw==GREDRAW_REDRAW,gpr->gpr_Redraw==GREDRAW_UPDATE);
/*
	BGUI_PostRender(cl,obj,gpr);
*/
	D(bug("BarClassRender Done\n"));
	return(result);
}
METHOD_END

METHOD(BarClassGoActive, struct gpInput *,gpi)
{
	GD *gd=INST_DATA(cl,obj);
	ULONG result;

	D(bug("BarClassGoActive\n"));
	if((result=DoSuperMethodA(cl,obj,(Msg)gpi))==GMR_NOREUSE
	&& !(((struct Gadget *)obj)->Flags & GFLG_DISABLED)
	&& ((gd->Horizontal
	&& gd->HorizontalSize)
	|| (gd->Vertical
	&& gd->VerticalSize))
	&& gpi->gpi_IEvent)
	{
		LONG x,y;

		ComputeHitBox(obj,gd);
		ComputeBarBox(obj,gd);
		x=gpi->gpi_Mouse.X+gd->Left;
		y=gpi->gpi_Mouse.Y+gd->Top;
		D(bug("Position x-%ld, y-%ld\n",x,y));
		if(PointInBox(gd->HitBox,x,y))
		{
			ULONG horizontal_position,vertical_position;

			horizontal_position=gd->HorizontalPosition;
			vertical_position=gd->VerticalPosition;
			if(PointInBox(gd->KnobBox,x,y))
			{
				gd->Holding=TRUE;
				gd->HoldingLeft=x-gd->KnobBox.Left;
				gd->HoldingTop=y-gd->KnobBox.Top;
				gd->StartHorizontalPosition=gd->HorizontalPosition;
				gd->StartVerticalPosition=gd->VerticalPosition;
				DrawGadget(obj,gd,gpi->gpi_GInfo,FALSE,FALSE,TRUE);
				result=GMR_MEACTIVE;
			}
			else
			{
				if(gd->Horizontal
				&& gd->HorizontalSize)
				{
					if(x<gd->KnobBox.Left)
						horizontal_position=(horizontal_position<gd->HorizontalPage ? 0 : horizontal_position-gd->HorizontalPage);
					else
					{
						if(x>=gd->KnobBox.Left+gd->KnobBox.Width)
							horizontal_position=(gd->HorizontalPage>gd->HorizontalTotal-gd->HorizontalSize-gd->HorizontalPosition ? gd->HorizontalTotal-gd->HorizontalSize : gd->HorizontalPosition+gd->HorizontalPage);
					}
				}
				if(gd->Vertical
				&& gd->VerticalSize)
				{
					if(y<gd->KnobBox.Top)
						vertical_position=(vertical_position<gd->VerticalPage ? 0 : vertical_position-gd->VerticalPage);
					else
					{
						if(y>=gd->KnobBox.Top+gd->KnobBox.Height)
							vertical_position=(gd->VerticalPage>gd->VerticalTotal-gd->VerticalSize-gd->VerticalPosition ? gd->VerticalTotal-gd->VerticalSize : gd->VerticalPosition+gd->VerticalPage);
					}
				}
			}
			D(bug("Horizontal Position-%lu\nVertical Position-%lu\n\n",horizontal_position,vertical_position));
			if(UpdateKnob(obj,gd,gpi->gpi_GInfo,0,horizontal_position,vertical_position,FALSE,TRUE))
				result|=GMR_VERIFY;
		}
	}
	return(result);
}
METHOD_END

METHOD(BarClassHandleInput, struct gpInput *,gpi)
{
	GD *gd=INST_DATA(cl,obj);
	struct gpInput copy;
	ULONG result;

	D(bug("BarClassHandleInput\n"));
	copy= *gpi;
	copy.MethodID=BASE_DRAGGING;
	result=GMR_MEACTIVE;
	switch(DoMethodA(obj,(Msg)&copy))
	{
		case BDR_NONE:
			if(gpi->gpi_IEvent->ie_Class==IECLASS_RAWMOUSE)
			{
				switch(gpi->gpi_IEvent->ie_Code)
				{
					case SELECTUP:
						DrawGadget(obj,gd,gpi->gpi_GInfo,TRUE,FALSE,TRUE);
						NotifyKnob(obj,gd,gpi->gpi_GInfo,0,TRUE,TRUE);
						result=GMR_NOREUSE|GMR_VERIFY;
						break;
					case MENUDOWN:
						result=GMR_REUSE;
						if(gd->HorizontalPosition!=gd->StartHorizontalPosition
						|| gd->VerticalPosition!=gd->StartVerticalPosition)
						{
							gd->HorizontalPosition=gd->StartHorizontalPosition;
							gd->VerticalPosition=gd->StartVerticalPosition;
							DrawGadget(obj,gd,gpi->gpi_GInfo,TRUE,TRUE,TRUE);
							NotifyKnob(obj,gd,gpi->gpi_GInfo,0,TRUE,TRUE);
							result|=GMR_VERIFY;
						}
						else
							DrawGadget(obj,gd,gpi->gpi_GInfo,TRUE,FALSE,TRUE);
						break;
					default:
					{
						LONG position;
						ULONG horizontal_position,vertical_position;
						BOOL redraw=FALSE;

						if(gd->HitBox.Width>gd->KnobBox.Width)
						{
							if(gd->HorizontalTotal>gd->HorizontalSize
							&& (position=gpi->gpi_Mouse.X+((struct Gadget *)obj)->LeftEdge-gd->HoldingLeft-gd->HitBox.Left)>0)
							{
								if(position>gd->HitBox.Width-gd->KnobBox.Width)
								{
									position=gd->HitBox.Width-gd->KnobBox.Width;
									horizontal_position=gd->HorizontalTotal-gd->HorizontalSize;
								}
								else
									horizontal_position=(double)position*(double)(gd->HorizontalTotal-gd->HorizontalSize)/(double)(gd->HitBox.Width-gd->KnobBox.Width);
							}
							else
							{
								position=0;
								horizontal_position=0;
							}
							if(gd->KnobBox.Left!=position+gd->HitBox.Left)
							{
								gd->KnobBox.Left=position+gd->HitBox.Left;
								redraw=TRUE;
							}
						}
						else
							horizontal_position=gd->HorizontalPosition;
						if(gd->HitBox.Height>gd->KnobBox.Height)
						{
							if(gd->VerticalTotal>gd->VerticalSize
							&& (position=gpi->gpi_Mouse.Y+((struct Gadget *)obj)->TopEdge-gd->HoldingTop-gd->HitBox.Top)>0)
							{
								if(position>gd->HitBox.Height-gd->KnobBox.Height)
								{
									position=gd->HitBox.Height-gd->KnobBox.Height;
									vertical_position=gd->VerticalTotal-gd->VerticalSize;
								}
								else
									vertical_position=(double)position*(double)(gd->VerticalTotal-gd->VerticalSize)/(double)(gd->HitBox.Height-gd->KnobBox.Height);
							}
							else
							{
								position=0;
								vertical_position=0;
							}
							if(gd->KnobBox.Top!=position+gd->HitBox.Top)
							{
								gd->KnobBox.Top=position+gd->HitBox.Top;
								redraw=TRUE;
							}
						}
						else
							vertical_position=gd->VerticalPosition;
						if(UpdateKnob(obj,gd,gpi->gpi_GInfo,OPUF_INTERIM,horizontal_position,vertical_position,FALSE,FALSE)==FALSE
						&& redraw)
							DrawGadget(obj,gd,gpi->gpi_GInfo,FALSE,FALSE,TRUE);
						break;
					}
				}
			}
			break;
		case BDR_DROP:
		case BDR_CANCEL:
			result=GMR_NOREUSE;
			break;
		case BDR_DRAGPREPARE:
			DrawGadget(obj,gd,gpi->gpi_GInfo,TRUE,FALSE,TRUE);
			break;
		case BDR_DRAGGING:
		default:
			break;
	}
	return(result);
}
METHOD_END

METHOD(BarClassGoInactive, struct gpGoInactive *,ggi)
{
	GD *gd=INST_DATA(cl,obj);

	D(bug("BarClassGoInactive\n"));
	DrawGadget(obj,gd,ggi->gpgi_GInfo,TRUE,FALSE,TRUE);
	return(DoSuperMethodA(cl,obj,(Msg)ggi));
}
METHOD_END

METHOD(BarClassDimensions, struct grmDimensions *,dim)
{
	GD *gd=INST_DATA(cl,obj);
	ULONG result;

	D(bug("BarClassDimensions\n"));
	result=DoSuperMethodA(cl,obj,(Msg)dim);
	*(dim->grmd_MinSize.Width)+=gd->MinimumKnobWidth+2*gd->KnobImageFrameWidth+1;
	*(dim->grmd_MinSize.Height)+=gd->MinimumKnobHeight+2*gd->KnobImageFrameHeight+1;
	D(bug("BarClassDimensions Done\n"));
	return(result);
}
METHOD_END

static DPFUNC ClassFunctions[] = {
	OM_NEW,          (FUNCPTR)BarClassNew,
	OM_DISPOSE,      (FUNCPTR)BarClassDispose,
	OM_SET,          (FUNCPTR)BarClassSet,
	OM_GET,          (FUNCPTR)BarClassGet,
	OM_UPDATE,       (FUNCPTR)BarClassSet,
	GM_RENDER,       (FUNCPTR)BarClassRender,
	GM_GOACTIVE,     (FUNCPTR)BarClassGoActive,
	GM_HANDLEINPUT,  (FUNCPTR)BarClassHandleInput,
	GM_GOINACTIVE,   (FUNCPTR)BarClassGoInactive,
	GRM_DIMENSIONS,  (FUNCPTR)BarClassDimensions,
	DF_END,          NULL
};

void __exit(int errcode)
{
}

#ifdef __AROS__
UBYTE _LibName[]   = "bgui_bar.gadget";
UBYTE _LibID[]     = "\0$VER: bgui_bar.gadget 41.10 (3.5.00) ©1996 Manuel Lemos ©2000 BGUI Developers Team";
UWORD _LibVersion  = 41;
UWORD _LibRevision = 10;
#else
extern UBYTE _LibName[]   = "bgui_bar.gadget";
extern UBYTE _LibID[]     = "\0$VER: bgui_bar.gadget 41.10 (3.5.00) ©1996 Manuel Lemos ©2000 BGUI Developers Team";
extern UWORD _LibVersion  = 41;
extern UWORD _LibRevision = 10;
#endif
/*--------------------------------LIBARY CODE FOLLOWS-----------------------------------*/

/*
 * The following code is used for the shared library.
 */
static Class *ClassBase = NULL;

/*
 * Initialize the class.
 */
SAVEDS ASM Class *BGUI_ClassInit(void)
{
	ClassBase=BGUI_MakeClass(
		CLASS_SuperClassBGUI,BGUI_BASE_GADGET,
		CLASS_ObjectSize,sizeof(GD),
		CLASS_DFTable,ClassFunctions,
		TAG_DONE);
	return(ClassBase);
}

/*
 * Called each time the library is closed. It simply closes
 * the required libraries and frees the class.
 */
SAVEDS ASM BOOL BGUI_ClassFree(void)
{
	return(BGUI_FreeClass(ClassBase));
}
