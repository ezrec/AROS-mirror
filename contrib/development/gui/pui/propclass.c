#include <OSIncludes.h>

#ifndef _AROS
#pragma header
#endif

#include "pui.h"
#include "classmacros.h"
#include "propclass.h"

ULONG PropInstanceSize(void)
{
	return sizeof(struct propdata);
}

ULONG PropDispatcher(struct IClass *cl,Object *obj,Msg msg)
{
	struct PUI_AreaData	*adata;
	struct propdata		*data;

	ULONG						 rc;
	
	rc = 0;

	if (msg->MethodID != OM_NEW)
	{
		data = INST_DATA(cl,obj);
		adata = INST_DATA(areaclass,obj);
	}

	switch(msg->MethodID)
	{
		case OM_NEW:
			if ((obj = (Object *)DoSuperMethodA(cl,obj,msg)))
			{
				data = INST_DATA(cl,obj);
				
				data->ishoriz = GetTagData(PUIA_Prop_Horiz,FALSE,ATTRLIST);

				if (!(data->propobj = NewObject(0,"propgclass",
															PGA_Freedom,data->ishoriz ? FREEHORIZ : FREEVERT,
															PGA_NewLook,TRUE,
															PGA_Borderless,TRUE,
															TAG_MORE,ATTRLIST)))
				{
					CoerceMethod(cl,obj,OM_DISPOSE);
				} else {
					rc = (ULONG)obj;
				}
			}
			break;

		case OM_DISPOSE:
			if (data->propobj) DisposeObject(data->propobj);
			DoSuperMethodA(cl,obj,msg);
			break;

		case PUIM_MINMAX:
			DoSuperMethodA(cl,obj,msg);

			if (data->ishoriz)
			{
				MINMAXMSG->minmax->minwidth += PROPWIDTH;
				MINMAXMSG->minmax->defwidth += PROPWIDTH;
				MINMAXMSG->minmax->maxwidth += PUI_MAXMAX;
				
				MINMAXMSG->minmax->minheight += PROPHEIGHT;
				MINMAXMSG->minmax->defheight += PROPHEIGHT;
				MINMAXMSG->minmax->maxheight += PROPHEIGHT;
			} else {
				MINMAXMSG->minmax->minwidth += PROPWIDTH;
				MINMAXMSG->minmax->defwidth += PROPWIDTH;
				MINMAXMSG->minmax->maxwidth += PROPWIDTH;
				
				MINMAXMSG->minmax->minheight += PROPHEIGHT;
				MINMAXMSG->minmax->defheight += PROPHEIGHT;
				MINMAXMSG->minmax->maxheight += PUI_MAXMAX;				
			}
			
			break;

		case PUIM_LAYOUT:
			DoSuperMethodA(cl,obj,msg);
			SetAttrs(data->propobj,GA_Left,LAYOUTMSG->box.Left + adata->offsetl,
										  GA_Top,LAYOUTMSG->box.Top + adata->offseto,
										  GA_Width,LAYOUTMSG->box.Width - adata->offsetl - adata->offsetr,
										  GA_Height,LAYOUTMSG->box.Height - adata->offseto - adata->offsetu,
										  TAG_DONE);
											 
			break;
		
		case GM_RENDER:
			if (RENDERMSG->gpr_RPort && PUI_MayRender(obj))
			{
				if (RENDERMSG->gpr_Redraw == GREDRAW_REDRAW)
				{
					DoSuperMethodA(cl,obj,msg);
				};
				DoMethodA(data->propobj,msg);
			}
			break;
			
		case GM_GOACTIVE:
		case GM_HANDLEINPUT:
			INPUTMSG->gpi_Mouse.X -= adata->offsetl;
			INPUTMSG->gpi_Mouse.Y -= adata->offseto;

			rc = DoMethodA(data->propobj,msg);
			if (rc & GMR_VERIFY)
			{
				*INPUTMSG->gpi_Termination = GAD->GadgetID;
			}
			break;

		case GM_GOINACTIVE:
		case OM_SET:
		case OM_GET:
		case OM_UPDATE:
		case OM_NOTIFY:
			rc = DoMethodA(data->propobj,msg);
			break;


		default:
			rc = DoSuperMethodA(cl,obj,msg);
			break;

	} /* switch(msg->MethodID) */
	
	return rc;	
}


