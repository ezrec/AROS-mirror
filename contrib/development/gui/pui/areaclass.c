#include <OSIncludes.h>

#ifndef _AROS
#pragma header
#endif

#include <string.h>
#include <stdio.h>

#include "pui.h"
#include "classmacros.h"
#include "areaclass.h"
#include "groupclass.h"

static struct TagItem flagtags[] =
{
	{PUIA_FrameRecessed,PUIF_Area_Recessed},
	{PUIA_FramePhantomHoriz,PUIF_Area_FramePhantomHoriz},
	{PUIA_FillArea,PUIF_Area_FillArea},
	{PUIA_ShowSelState,PUIF_Area_ShowSelState},
	{TAG_DONE}
};

static void RenderFrameTitle(struct IClass *cl,Object *obj,struct RastPort *rp)
{
	struct groupdata *data;
	struct PUI_AreaData *adata;

	struct TextExtent te;

	WORD freespace,chars,x,y;

	data = INST_DATA(cl,obj);
	adata = INST_DATA(areaclass,obj);
	
	freespace = GAD->Width - (GROUPTITLEBORDER + GROUPTITLESPACE) * 2;
		
	chars = TextFit(rp,
			  			 adata->frametitle,
			  			 strlen(adata->frametitle),
			  			 &te,
			  			 0,
			  			 1,
			  			 freespace,
			  			 1000);

	if (chars > 1)
	{
		switch(adata->frametitlealign)
		{
			case PUIV_Align_Left:
				x = GAD->LeftEdge + GROUPTITLEBORDER + GROUPTITLESPACE;
				break;
				
			case PUIV_Align_Right:
				x = GAD->LeftEdge + GAD->Width - GROUPTITLEBORDER - GROUPTITLESPACE - te.te_Width;
				break;
				
			default:				
				x = GAD->LeftEdge + (GAD->Width - te.te_Width) / 2;
				break;
		}

		y = GAD->TopEdge + adata->ri->fontbaseline;
		
		SetDrMd(rp,JAM1);
		SetSoftStyle(rp,0,AskSoftStyle(rp));
		SetAPen(rp,adata->ri->pens[BACKGROUNDPEN]);
		RectFill(rp,x - GROUPTITLESPACE,
						GAD->TopEdge,
						x + te.te_Width + GROUPTITLESPACE,
						GAD->TopEdge + adata->ri->fontheight);

		if (grouptitles_3d)
		{
			Move(rp,x+1,y+1);
			SetAPen(rp,adata->ri->pens[SHADOWPEN]);
			Text(rp,adata->frametitle,chars);
		}

		Move(rp,x,y);
		SetAPen(rp,adata->ri->pens[SHINEPEN]);
		Text(rp,adata->frametitle,chars);

	} /* if (chars > 1) */
}


ULONG AreaInstanceSize(void)
{
	return sizeof(struct PUI_AreaData);
}

ULONG AreaDispatcher(struct IClass *cl,Object *obj,Msg msg)
{
	struct PUI_AreaData 	*data;
	struct groupdata	*gdata;
	struct TagItem 	*ti;
	struct RastPort	*rp;
	struct IBox			insidebox;
	Object				*o;
	WORD					x,y;
	BOOL					selected;
	LONG					l;

	ULONG rc;
	
	rc = 0;
	if (msg->MethodID != OM_NEW) data = INST_DATA(cl,obj);

	switch (msg->MethodID)
	{
		case OM_NEW:
			if ((rc = DoSuperMethodA(cl,obj,msg)))
			{
				obj = (Object *)rc;

				data = INST_DATA(cl,obj);
				memset(data,0,sizeof(*data));
				
				data->frametype = GetTagData(PUIA_Frame,PUIV_Frame_None,ATTRLIST);
				data->frametitle = (char *)GetTagData(PUIA_FrameTitle,0,ATTRLIST);
				data->frametitlealign = GetTagData(PUIA_FrameTitleAlign,PUIV_Align_Center,ATTRLIST);

				data->flags = PackBoolTags(PUIF_Area_FillArea | PUIF_Area_ShowSelState,ATTRLIST,flagtags);

				if ((l = GetTagData(PUIA_HorizAlign,-1,ATTRLIST)) != -1)
				{
					if (l == PUIV_Align_Left)
					{
						data->flags |= PUIF_Area_AlignLeft;
					} else if (l == PUIV_Align_Right)
					{
						data->flags |= PUIF_Area_AlignRight;
					}
				}
				if ((l = GetTagData(PUIA_VertAlign,-1,ATTRLIST)) != -1)
				{
					if (l == PUIV_Align_Top)
					{
						data->flags |= PUIF_Area_AlignTop;
					} else if (l == PUIV_Align_Bottom)
					{
						data->flags |= PUIF_Area_AlignBottom;
					}
				}
				data->horizweight = GetTagData(PUIA_HorizWeight,100,ATTRLIST);
				data->vertweight  = GetTagData(PUIA_VertWeight,100,ATTRLIST);
				if ((l = GetTagData(PUIA_Weight,100,ATTRLIST)) != 100)
				{
					data->horizweight =
					data->vertweight  = l;
				}
			}
			break;
		
		case OM_GET:
			switch(GETMSG->opg_AttrID)
			{
				case PUIA_Area_RenderInfo:
					*(struct PUI_RenderInfo **)GETMSG->opg_Storage = data->ri;
					rc = TRUE;
					break;
				
				case PUIA_Parent:
					*(Object **)GETMSG->opg_Storage = data->parent;
					rc = TRUE;
					break;
					
				default:
					rc = DoSuperMethodA(cl,obj,msg);
					break;
				
			}
			break;
		
		case OM_SET:
			if ((ti = FindTagItem(PUIA_Parent,SETMSG->ops_AttrList)))
			{
				data->parent = (Object *)ti->ti_Data;
			}
			rc = DoSuperMethodA(cl,obj,msg);
			break;
			
		case PUIM_SETUP:
			data->ri = SETUPMSG->ri;
			data->root = SETUPMSG->root;
			
			x = data->frametype;

			if (!(data->flags & PUIF_Area_FramePhantomHoriz))
			{
				data->offsetl = frameinfo[x].borderleft +
									 frameinfo[x].spaceleft;
	
				data->offsetr = frameinfo[x].borderright +
									 frameinfo[x].spaceright;
			}

			data->offseto = (data->frametitle ? data->ri->fontheight : frameinfo[x].bordertop) +
								 frameinfo[x].spacetop;
					 

			data->offsetu = frameinfo[x].borderbottom +
								 frameinfo[x].spacebottom;
			break;

		case PUIM_MINMAX:
			x = data->offsetl + data->offsetr;

			y = data->offseto + data->offsetu;

			data->minmax.minwidth += x;  
			data->minmax.defwidth += x;
			data->minmax.maxwidth += x;

			data->minmax.minheight += y;
			data->minmax.defheight += y;
			data->minmax.maxheight += y;
			break;
		
		case PUIM_LAYOUT:
			GAD->LeftEdge = LAYOUTMSG->box.Left;
			GAD->TopEdge  = LAYOUTMSG->box.Top;
			GAD->Width    = LAYOUTMSG->box.Width;
			GAD->Height	  = LAYOUTMSG->box.Height;
			break;
		
		case PUIM_DONOTRENDER:
			data->renderlock++;
			break;
			
		case PUIM_MAYRENDER:
			data->renderlock--;
			break;

		case GM_HITTEST:
			o = data->root;
			gdata = INST_DATA(groupclass,o);
			gdata->activeobj = obj;
			rc = GMR_GADGETHIT;
			break;

		case GM_HELPTEST:
			rc = 0;
			break;

		case GM_GOACTIVE:
			*INPUTMSG->gpi_Termination = GAD->GadgetID;
			rc = GMR_MEACTIVE;
			break;

		case GM_RENDER:
			if ((rp = RENDERMSG->gpr_RPort) && PUI_MayRender(obj))
			{
				selected = ((GAD->Flags & GFLG_SELECTED) && (data->flags & PUIF_Area_ShowSelState)) ? TRUE : FALSE;

				insidebox = *(struct IBox *)&GAD->LeftEdge;
				if (data->frametitle)
				{
					if (grouptitles_center)
					{
						y = data->ri->fontheight / 2;
					} else {
						y = data->ri->fontbaseline - frameinfo[data->frametype].bordertop + 1;
					}
					
					insidebox.Top += y;
					insidebox.Height -= y;
				}

				if (!(data->flags & PUIF_Area_FramePhantomHoriz))
				{
					DrawFrame(obj,rp,&insidebox,data->frametype,selected);
				}
				
				if (data->flags & PUIF_Area_FillArea)
				{
					ShrinkBox(&insidebox,
								 frameinfo[data->frametype].borderleft,
								 frameinfo[data->frametype].bordertop,
								 frameinfo[data->frametype].borderright,
								 frameinfo[data->frametype].borderbottom);
		
					SetDrMd(rp,JAM2);
					SetAPen(rp,data->ri->pens[selected ? FILLPEN : BACKGROUNDPEN]);
					RectFill(rp,insidebox.Left,
									insidebox.Top,
									insidebox.Left + insidebox.Width - 1,
									insidebox.Top + insidebox.Height - 1);
				}

				if (data->frametitle && (GAD->Width > (GROUPTITLEBORDER * 2 + GROUPTITLESPACE * 2)))
				{
					RenderFrameTitle(cl,obj,rp);
				}
			
			} /* if ((rp = RENDERMSG->gpr_RPort)) */
			break;
			
		default:
			rc = DoSuperMethodA(cl,obj,msg);
			break;

	} /* switch (msg->MethodID) */
	
	return rc;
}

