#include <OSIncludes.h>

#ifndef _AROS
#pragma header
#endif

#include <string.h>

#include "pui.h"
#include "classmacros.h"
#include "groupclass.h"

#define GADO ((struct Gadget *)o)

static struct TagItem flagtags[] =
{
	{PUIA_Group_SameWidth,PUIF_Group_SameWidth},
	{PUIA_Group_SameHeight,PUIF_Group_SameHeight},
	{PUIA_Group_SameSize,PUIF_Group_SameWidth | PUIF_Group_SameHeight},
	{PUIA_Group_IsRoot,PUIF_Group_IsRoot},
	{PUIA_Group_Horiz,PUIF_Group_IsHoriz},
	{PUIA_Group_PageMode,PUIF_Group_PageMode},
	{TAG_DONE}
};

static void mgadLayout(struct IClass *cl,Object *obj,struct gpLayout *msg)
{
	struct Window		*win;
	struct LayoutMsg	lmsg;

	win = msg->gpl_GInfo->gi_Window;
	
	lmsg.MethodID   = PUIM_LAYOUT;
	lmsg.gi			 = 0;
	lmsg.box.Left   = win->BorderLeft;
	lmsg.box.Top    = win->BorderTop;
	lmsg.box.Width  = win->Width - win->BorderLeft - win->BorderRight;
	lmsg.box.Height = win->Height - win->BorderTop - win->BorderBottom;
								
	DoMethodA(obj,(Msg)&lmsg);
								
}

static void resetlayout(struct IClass *cl,Object *obj)
{
	struct groupdata		*data;
	struct PUI_AreaData	*adata;
	Object 					*o,*objstate;
	
	data = INST_DATA(cl,obj);

	objstate = (Object *)data->childs.mlh_Head;
	while ((o = NextObject(&objstate)))
	{
		adata = INST_DATA(areaclass,o);

		adata->flags &= (~PUIF_Area_Layouted);

		adata->layoutwidth  = adata->minmax.minwidth;
		adata->layoutheight = adata->minmax.minheight;
	}
};

static void layoutpagemode(struct IClass *cl,Object *obj,struct LayoutMsg *msg)
{
	struct groupdata		*data;
	struct PUI_AreaData 	*adata;
	struct IBox 			insidebox;
	struct LayoutMsg		layoutmsg;
	Object					*o,*objstate;

	data  = INST_DATA(cl,obj);
	adata = INST_DATA(areaclass,obj);
	
	insidebox.Left   = msg->box.Left + adata->offsetl;
	insidebox.Top    = msg->box.Top  + adata->offseto;
	insidebox.Width  = msg->box.Width - adata->offsetl - adata->offsetr;
	insidebox.Height = msg->box.Height - adata->offseto - adata->offsetu;

	layoutmsg.MethodID = PUIM_LAYOUT;
	layoutmsg.gi = 0;

	objstate = (Object *)data->childs.mlh_Head;
	
	while ((o = NextObject(&objstate)))
	{
		adata = INST_DATA(areaclass,o);

		layoutmsg.box.Width  = SelectWidth(&insidebox,&adata->minmax);
		layoutmsg.box.Height = SelectHeight(&insidebox,&adata->minmax);
		layoutmsg.box.Left   = SelectLeft(o,&insidebox,&adata->minmax);
		layoutmsg.box.Top 	= SelectTop(o,&insidebox,&adata->minmax);
	
		DoMethodA(o,(Msg)&layoutmsg);
	}
	
}

static void layouthoriz(struct IClass *cl,Object *obj,struct LayoutMsg *msg)
{
	struct groupdata		*data;
	struct PUI_AreaData 	*adata;
	struct IBox 			insidebox,gbox;
	struct LayoutMsg		layoutmsg;
	Object					*o,*objstate;
	LONG						l,freewidth,swidth,weights;
	WORD						x,count;

	data  = INST_DATA(cl,obj);
	adata = INST_DATA(areaclass,obj);
	
	insidebox.Left   = msg->box.Left + adata->offsetl;
	insidebox.Top    = msg->box.Top  + adata->offseto;
	insidebox.Width  = msg->box.Width - adata->offsetl - adata->offsetr;
	insidebox.Height = msg->box.Height - adata->offseto - adata->offsetu;

	layoutmsg.MethodID = PUIM_LAYOUT;
	layoutmsg.gi = 0;

	x = insidebox.Left;
	layoutmsg.box.Top  = insidebox.Top;
	
	count = 0;
	objstate = (Object *)data->childs.mlh_Head;
	
	// swidth is for groups with samewidth flag set
	// using fixed point math for necessary precision

	swidth = ((insidebox.Width + data->spacing) << 16) / data->numchilds;
	gbox.Width = (insidebox.Width - ((data->numchilds - 1) * data->spacing)) / data->numchilds;

	if (data->flags & PUIF_Group_SameWidth)
	{
		while ((o = NextObject(&objstate)))
		{
			adata = INST_DATA(areaclass,o);
	
			layoutmsg.box.Height = SelectHeight(&insidebox,&adata->minmax);
			layoutmsg.box.Top 	= SelectTop(o,&insidebox,&adata->minmax);
	
			x = insidebox.Left + ((count * swidth) >> 16);
			gbox.Left = x;
				
			layoutmsg.box.Width = SelectWidth(&gbox,&adata->minmax);
			layoutmsg.box.Left  = SelectLeft(o,&gbox,&adata->minmax);

			DoMethodA(o,(Msg)&layoutmsg);
			count++;
		}
	} else {
	
		resetlayout(cl,obj);
		
		freewidth = insidebox.Width - (data->numchilds - 1) * data->spacing;
		
		// count weights and mark objects with
		// fixed width as done (which also means
		// that the weight of such an object isn't
		// counted either!)
		
		weights = 0;
		while ((o = NextObject(&objstate)))
		{
			adata = INST_DATA(areaclass,o);
			
			if (adata->layoutwidth == adata->minmax.maxwidth)
			{
				adata->flags |= PUIF_Area_Layouted;
				freewidth -= adata->layoutwidth;
			} else {
				weights += adata->horizweight;
			}
		}
		if (weights < 1) weights = 1;

		// handle objects that have recieved more
		// weight than requested even when using
		// their minwidth
 
		objstate = (Object *)data->childs.mlh_Head;
		while ((o = NextObject(&objstate)))
		{
			adata = INST_DATA(areaclass,o);

			if (!(adata->flags & PUIF_Area_Layouted))
			{
				l = freewidth * adata->horizweight / weights;
				if (adata->layoutwidth >= l)
				{
					adata->flags |= PUIF_Area_Layouted;
					freewidth -= adata->layoutwidth;
					weights   -= adata->horizweight;

					// restart loop
					objstate = (Object *)data->childs.mlh_Head;
				}
			}
		}
		
		// now handle the remaining objects
		
		objstate = (Object *)data->childs.mlh_Head;
		while ((o = NextObject(&objstate)))
		{
			adata = INST_DATA(areaclass,o);

			if (!(adata->flags & PUIF_Area_Layouted))
			{
				l = freewidth * adata->horizweight / weights;
				if (l > adata->minmax.maxwidth)
				{
					l = adata->minmax.maxwidth;
				}
				adata->flags |= PUIF_Area_Layouted;
				adata->layoutwidth = l;

				freewidth -= adata->layoutwidth;
				weights   -= adata->horizweight;
			}
		}
		
		// done. now send layout messages
		
		objstate = (Object *)data->childs.mlh_Head;
		while ((o = NextObject(&objstate)))
		{
			adata = INST_DATA(areaclass,o);

			layoutmsg.box.Height = SelectHeight(&insidebox,&adata->minmax);
			layoutmsg.box.Top 	= SelectTop(o,&insidebox,&adata->minmax);
	
			layoutmsg.box.Width = adata->layoutwidth;
			layoutmsg.box.Left  = x;

			DoMethodA(o,(Msg)&layoutmsg);
			
			x += (adata->layoutwidth + data->spacing);
		}
		
	}
}

static void layoutvert(struct IClass *cl,Object *obj,struct LayoutMsg *msg)
{
	struct groupdata		*data;
	struct PUI_AreaData	*adata;
	struct IBox 			insidebox,gbox;
	struct LayoutMsg 		layoutmsg;
	Object					*o,*objstate;
	LONG						l,freeheight,sheight,weights;

	WORD						y,count;

	data  = INST_DATA(cl,obj);
	adata = INST_DATA(areaclass,obj);
	
	insidebox.Left   = msg->box.Left + adata->offsetl;
	insidebox.Top    = msg->box.Top  + adata->offseto;
	insidebox.Width  = msg->box.Width - adata->offsetl - adata->offsetr;
	insidebox.Height = msg->box.Height - adata->offseto - adata->offsetu;

	layoutmsg.MethodID = PUIM_LAYOUT;
	layoutmsg.gi = 0;
	
	y = insidebox.Top;

	// sheight is for groups with sameheight flag set
	// using fixed point math for necessary precision

	sheight = ((insidebox.Height + data->spacing) << 16) / data->numchilds;
	gbox.Height = (insidebox.Height - ((data->numchilds - 1) * data->spacing)) / data->numchilds;

	count = 0;
	objstate = (Object *)data->childs.mlh_Head;
	
	if (data->flags & PUIF_Group_SameHeight)
	{
		while ((o = NextObject(&objstate)))
		{
			adata = INST_DATA(areaclass,o);
	
			layoutmsg.box.Width = SelectWidth(&insidebox,&adata->minmax);
			layoutmsg.box.Left  = SelectLeft(o,&insidebox,&adata->minmax);
	
			y = insidebox.Top + ((count * sheight) >> 16);
			gbox.Top = y;
				
			layoutmsg.box.Height = SelectHeight(&gbox,&adata->minmax);
			layoutmsg.box.Top    = SelectTop(o,&gbox,&adata->minmax);
		}
	
		DoMethodA(o,(Msg)&layoutmsg);
		count++;	
	} else {
		resetlayout(cl,obj);
		
		freeheight = insidebox.Height - (data->numchilds - 1) * data->spacing;
		
		// count weights and mark objects with
		// fixed width as done (which also means
		// that the weight of such an object isn't
		// counted either!)
		
		weights = 0;
		while ((o = NextObject(&objstate)))
		{
			adata = INST_DATA(areaclass,o);
			
			if (adata->layoutheight == adata->minmax.maxheight)
			{
				adata->flags |= PUIF_Area_Layouted;
				freeheight -= adata->layoutwidth;
			} else {
				weights += adata->vertweight;
			}
		}
		if (weights < 1) weights = 1;

		// handle objects that have recieved more
		// weight than requested even when using
		// their minwidth
 
		objstate = (Object *)data->childs.mlh_Head;
		while ((o = NextObject(&objstate)))
		{
			adata = INST_DATA(areaclass,o);

			if (!(adata->flags & PUIF_Area_Layouted))
			{
				l = freeheight * adata->vertweight / weights;
				if (adata->layoutheight >= l)
				{
					adata->flags |= PUIF_Area_Layouted;
					freeheight -= adata->layoutheight;
					weights   -= adata->vertweight;

					// restart loop
					objstate = (Object *)data->childs.mlh_Head;
				}
			}
		}
		
		// now handle the remaining objects
		
		objstate = (Object *)data->childs.mlh_Head;
		while ((o = NextObject(&objstate)))
		{
			adata = INST_DATA(areaclass,o);

			if (!(adata->flags & PUIF_Area_Layouted))
			{
				l = freeheight * adata->vertweight / weights;
				if (l > adata->minmax.maxheight)
				{
					l = adata->minmax.maxheight;
				}
				adata->flags |= PUIF_Area_Layouted;
				adata->layoutheight = l;

				freeheight -= adata->layoutheight;
				weights    -= adata->vertweight;
			}
		}
		
		// done. now send layout messages
		
		objstate = (Object *)data->childs.mlh_Head;
		while ((o = NextObject(&objstate)))
		{
			adata = INST_DATA(areaclass,o);

			layoutmsg.box.Width  = SelectWidth(&insidebox,&adata->minmax);
			layoutmsg.box.Left 	= SelectLeft(o,&insidebox,&adata->minmax);
	
			layoutmsg.box.Height = adata->layoutheight;
			layoutmsg.box.Top	   = y;

			DoMethodA(o,(Msg)&layoutmsg);
			
			y += (adata->layoutheight + data->spacing);
		}
	}
}

ULONG GroupInstanceSize(void)
{
	return sizeof(struct groupdata);
}

ULONG GroupDispatcher(struct IClass *cl,Object *obj,Msg msg)
{
	struct groupdata	 	*data;
	struct PUI_AreaData	*adata,*adatao;
	struct TagItem 	 	*ti,*tstate;
	struct PUI_MinMax		*mm,*msgmm;

	Object					*o,*newo,*objstate;
	WORD						a,b,x,y;
	LONG						l;

	ULONG 					rc;
	
	rc = 0;
	if (msg->MethodID != OM_NEW)
	{
		data = INST_DATA(cl,obj);
		adata = INST_DATA(areaclass,obj);
	}

	switch(msg->MethodID)
	{
		case OM_NEW:
			if ((rc = DoSuperMethodA(cl,obj,msg)))
			{
				data = INST_DATA(cl,rc);

				NewList((struct List *)&data->childs);

				data->activeobj	= 0;
				data->flags			= PackBoolTags(0,ATTRLIST,flagtags);
				data->spacing		= GetTagData(PUIA_Group_Spacing,data->flags & PUIF_Group_IsHoriz ? group_defaultspacingx : group_defaultspacingy,ATTRLIST);
				data->activepage	= GetTagData(PUIA_Group_ActivePage,0,ATTRLIST);
				
				ti = ATTRLIST;

				while ((ti = FindTagItem(PUIA_Group_Child,ti)))
				{
					o = (Object *)ti->ti_Data;
					if (!o)
					{
						CoerceMethod(cl,(Object *)rc,OM_DISPOSE);
						return 0;
					}
					
					DoMethod((Object *)rc,OM_ADDMEMBER,o);

					SetAttrs(o,PUIA_Parent,rc,
								  TAG_DONE);

					data->numchilds++;
					ti++;

				} /* while ((ti = FindTagItem(PUIA_Group_Child,ti))) */
				
				if (data->flags & PUIF_Group_PageMode)
				{
					a = 0;
					objstate = (Object *)data->childs.mlh_Head;
					while ((o = NextObject(&objstate)))
					{
						if (a == data->activepage)
						{
							data->activepageobj = o;
						} else {
							DoMethod(o,PUIM_DONOTRENDER);
						}
						a++;
					}
				} /* if (data->flags & PUIF_Group_PageMode) */

			} /* if ((rc = DoSuperMethodA(cl,obj,msg))) */
			break;

		case OM_DISPOSE:
			objstate = (Object *)data->childs.mlh_Head;
			while ((o = NextObject(&objstate)))
			{
				DoMethod(o,OM_REMOVE);
				DoMethodA(o,msg);
			}
			DoSuperMethodA(cl,obj,msg);
			break;

		case OM_ADDMEMBER:
			DoMethod(((struct opMember *)msg)->opam_Object,OM_ADDTAIL,&data->childs);
			break;
			
		case OM_REMMEMBER:
			DoMethod(((struct opMember *)msg)->opam_Object,OM_REMOVE);
			break;

		case OM_GET:
			rc = TRUE;
			switch (GETMSG->opg_AttrID)
			{
				case PUIA_Root_Dimensions:
					*(struct PUI_MinMax **)GETMSG->opg_Storage = &adata->minmax;
					break;
				
				case PUIA_Group_ActivePage:
					*(LONG *)GETMSG->opg_Storage = data->activepage;
					break;

				case PUIA_Group_ChildList:
					*(struct MinList **)GETMSG->opg_Storage = &data->childs;
					break;

				default:
					rc = DoSuperMethodA(cl,obj,msg);
			};
			break;
		
		case OM_SET:
			DoSuperMethodA(cl,obj,msg);

			tstate = ATTRLIST;
			while ((ti = NextTagItem(&tstate)))
			{
				switch(ti->ti_Tag)
				{
					case PUIA_Group_ActivePage:
						if ((ti->ti_Data != data->activepage) &&
							 (ti->ti_Data >= 0) &&
							 (ti->ti_Data < data->numchilds))
						{

							DoMethod(data->activepageobj,PUIM_DONOTRENDER);
							data->activepage = ti->ti_Data;
							
							a = 0;
							objstate = (Object *)data->childs.mlh_Head;
							while ((o = NextObject(&objstate)))
							{
								if (a == data->activepage)
								{
									data->activepageobj = o;

									DoMethod(o,PUIM_MAYRENDER);
									RenderObject_Redraw(obj,SETMSG->ops_GInfo);
									rc = TRUE;
									break;
								}
								a++;
							}

						}
						break;
				}
			}			
			break;
			
		case PUIM_SETUP:
			DoSuperMethodA(cl,obj,msg);
			
			objstate = (Object *)data->childs.mlh_Head;
			while ((o = NextObject(&objstate)))
			{
				DoMethodA(o,msg);
			}

			if (data->flags & PUIF_Group_IsRoot)
			{
				// should be "=" instead of "+=" since
				// there must not be any frame or program
				// specified innerspacing for the root
				// object ...

				adata->offsetl += window_defaultspacingl;
				adata->offsetr += window_defaultspacingr;
				adata->offseto += window_defaultspacingo;
				adata->offsetu += window_defaultspacingu;
			}
			break;

		case PUIM_MINMAX:

			/* ask all children about their size */

			objstate = (Object *)data->childs.mlh_Head;
			while ((o = NextObject(&objstate)))
			{
				adatao = INST_DATA(areaclass,o);

				DoMethod(o,PUIM_MINMAX,0,&adatao->minmax);
			}

			msgmm = MINMAXMSG->minmax;

			if (data->flags & PUIF_Group_PageMode)
			{
				/* This is a page mode group */

				memset(msgmm,0,sizeof(*msgmm));
				msgmm->maxwidth = msgmm->maxheight = PUI_MAXMAX;
				
				objstate = (Object *)data->childs.mlh_Head;
				while ((o = NextObject(&objstate)))
				{
					adatao = INST_DATA(areaclass,o);
					mm = &adatao->minmax;
					
					if (mm->minwidth > msgmm->minwidth) msgmm->minwidth = mm->minwidth;
					if (mm->defwidth > msgmm->defwidth) msgmm->defwidth = mm->defwidth;
					if (mm->maxwidth < msgmm->maxwidth) msgmm->maxwidth = mm->maxwidth;

					if (mm->minheight > msgmm->minheight) msgmm->minheight = mm->minheight;
					if (mm->defheight > msgmm->defheight) msgmm->defheight = mm->defheight;
					if (mm->maxheight < msgmm->maxheight) msgmm->maxheight = mm->maxheight;					
				}
				
			} /* if (data->flags & PUIF_Group_PageMode) */ else {

				/* This is a normal (non page mode) group */

				x = data->spacing * (data->numchilds - 1);
				if (data->flags & PUIF_Group_IsHoriz)
				{
					msgmm->minwidth = x; 
					msgmm->defwidth = x; 
					msgmm->maxwidth = x;
					
					msgmm->maxheight = PUI_MAXMAX;
				} else {
					msgmm->minheight = x;
					msgmm->defheight = x;
					msgmm->maxheight = x;
	
					msgmm->maxwidth = PUI_MAXMAX;
				}
	
				objstate = (Object *)data->childs.mlh_Head;
				
				while ((o = NextObject(&objstate)))
				{
					adatao = INST_DATA(areaclass,o);
					mm = &adatao->minmax;
	
					if (data->flags & PUIF_Group_IsHoriz)
					{
						if (!(data->flags & PUIF_Group_SameWidth))
						{
							msgmm->minwidth += mm->minwidth;
							msgmm->defwidth += mm->defwidth;
						}
						msgmm->maxwidth += mm->maxwidth;
	
						if (mm->minheight > msgmm->minheight)
							 msgmm->minheight = mm->minheight;
						if (mm->defheight > msgmm->defheight)
							 msgmm->defheight = mm->defheight;
						if (mm->maxheight < msgmm->maxheight)
							 msgmm->maxheight = mm->maxheight;
	
					} /* if (data->flags & PUIF_Group_IsHoriz) */ else {
	
						if (!(data->flags & PUIF_Group_SameHeight))
						{
							msgmm->minheight += mm->minheight;
							msgmm->defheight += mm->defheight;
						}
						msgmm->maxheight += mm->maxheight;
	
						if (mm->minwidth > msgmm->minwidth)
							 msgmm->minwidth = mm->minwidth;
						if (mm->defwidth > msgmm->defwidth)
							 msgmm->defwidth = mm->defwidth;
						if (mm->maxwidth < msgmm->maxwidth)
							 msgmm->maxwidth = mm->maxwidth;
					}
	
				} /* while ((o = NextObject(&objstate))) */
	
				if (data->flags & PUIF_Group_IsHoriz)
				{
					if (data->flags & PUIF_Group_SameWidth)
					{
						l = GreatestMinWidth(&data->childs) * data->numchilds;
	
						msgmm->minwidth += l;
						msgmm->defwidth += l;
					}
				} else {
					if (data->flags & PUIF_Group_SameHeight)
					{
						l = GreatestMinHeight(&data->childs) * data->numchilds;
						
						msgmm->minheight += l;
						msgmm->defheight += l;
					}
				}
			}
			
			DoSuperMethodA(cl,obj,msg);

			CorrectMinMax(msgmm);
			break;
		
		case PUIM_LAYOUT:
			DoSuperMethodA(cl,obj,msg);

			if (data->flags & PUIF_Group_PageMode)
			{
				layoutpagemode(cl,obj,LAYOUTMSG);
			} else {
				if (data->flags & PUIF_Group_IsHoriz)
				{
					layouthoriz(cl,obj,LAYOUTMSG);
				} else {
					layoutvert(cl,obj,LAYOUTMSG);
				}
			}
			break;
		
		case GM_HITTEST:
			x = HITTESTMSG->gpht_Mouse.X + GAD->LeftEdge;
			y = HITTESTMSG->gpht_Mouse.Y + GAD->TopEdge;

			if (data->flags & PUIF_Group_PageMode)
			{
				o = data->activepageobj;
				
				a = x - GADO->LeftEdge;
				b = y - GADO->TopEdge;
				
				if (a >= 0 && b >= 0 &&
					 a < GADO->Width && b < GADO->Height)
				{
					HITTESTMSG->gpht_Mouse.X = a;
					HITTESTMSG->gpht_Mouse.Y = b;
					rc = DoMethodA(o,msg);
				}
				
			} /* if (data->flags & PUIF_Group_PageMode) */ else {
			
				objstate = (Object *)data->childs.mlh_Head;
				while ((o = NextObject(&objstate)))
				{
					a = x - GADO->LeftEdge;
					b = y - GADO->TopEdge;
					
					if (a >= 0 && b >= 0 &&
						 a < GADO->Width && b < GADO->Height)
					{
						HITTESTMSG->gpht_Mouse.X = a;
						HITTESTMSG->gpht_Mouse.Y = b;
						rc = DoMethodA(o,msg);
	
						if (rc == GMR_GADGETHIT) break;
					}
				} /* while ((o = NextObject(&objstate))) */			
			}
			break;
		
		case GM_HANDLEINPUT:
		case GM_GOACTIVE:
			if (data->flags & PUIF_Group_IsRoot)
			{
				x = INPUTMSG->gpi_Mouse.X + GAD->LeftEdge;
				y = INPUTMSG->gpi_Mouse.Y + GAD->TopEdge;
	
				o = data->activeobj;
				
				INPUTMSG->gpi_Mouse.X = x - GADO->LeftEdge;
				INPUTMSG->gpi_Mouse.Y = y - GADO->TopEdge;

				rc = DoMethodA(o,msg);
				
				newo = 0;

				if (rc & GMR_NEXTACTIVE) newo = NextTabCycleObject(o);
				if (rc & GMR_PREVACTIVE) newo = PrevTabCycleObject(o);
				
				if ((newo != 0) && (newo != o))
				{
					DoMethod(o,GM_GOINACTIVE,INPUTMSG->gpi_GInfo,0);

					o = newo;
					data->activeobj = o;
					
					x -= GADO->LeftEdge;
					y -= GADO->TopEdge;

					rc = DoMethod(o,GM_GOACTIVE,
										 INPUTMSG->gpi_GInfo,
										 0,
										 INPUTMSG->gpi_Termination,
										 (x << 16) + y,
										 INPUTMSG->gpi_TabletData);
					
				} else {
					rc &= ~(GMR_NEXTACTIVE | GMR_PREVACTIVE);
				}

			}
			break;
		
		case GM_GOINACTIVE:
			if (data->flags & PUIF_Group_IsRoot)
			{
				DoMethodA(data->activeobj,msg);
				data->activeobj = 0;
			}
			break;

		case GM_RENDER:
			if (RENDERMSG->gpr_RPort && PUI_MayRender(obj))
			{
				if (RENDERMSG->gpr_Redraw == GREDRAW_REDRAW)
				{
					DoSuperMethodA(cl,obj,msg);
				}
	
				if (data->flags & PUIF_Group_PageMode)
				{
					DoMethodA(data->activepageobj,msg);
				} else {
					objstate = (Object *)data->childs.mlh_Head;
					while ((o = NextObject(&objstate)))
					{
						DoMethodA(o,msg);
					}
				}
			}
			break;

		case GM_LAYOUT:
			mgadLayout(cl,obj,(struct gpLayout *)msg);
			break;
		
		case PUIM_DONOTRENDER:
		case PUIM_MAYRENDER:
			DoSuperMethodA(cl,obj,msg);

			objstate = (Object *)data->childs.mlh_Head;
			while ((o = NextObject(&objstate)))
			{
				DoMethodA(o,msg);
			}			
			break;

		default:
			rc = DoSuperMethodA(cl,obj,msg);
			break;
	}
	
	return rc;
}

