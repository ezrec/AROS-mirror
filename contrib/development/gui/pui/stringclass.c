#include <OSIncludes.h>

#ifndef _AROS
#pragma header
#endif

#include <string.h>

#include "pui.h"
#include "classmacros.h"
#include "stringclass.h"

#define GADO ((struct Gadget *)o)

ULONG StringInstanceSize(void)
{
	return sizeof(struct stringdata);
}

ULONG StringDispatcher(struct IClass *cl,Object *obj,Msg msg)
{
	struct stringdata		*data;
	struct PUI_AreaData		*adata;
	struct TagItem			*ti;
	WORD						x,y;

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
				
				data->maxchars = GetTagData(PUIA_String_MaxChars,257,ATTRLIST);
				data->buffer = AllocVec(data->maxchars * 3,MEMF_PUBLIC|MEMF_CLEAR);
				if ((ti = FindTagItem(PUIA_String_Contents,ATTRLIST)))
				{
					if (data->buffer) strcpy(data->buffer,(char *)ti->ti_Data);
				}

				data->stringobj = NewObject(0,"strgclass",
											STRINGA_MaxChars,data->maxchars,
											STRINGA_Buffer,data->buffer,
											STRINGA_WorkBuffer,data->buffer + data->maxchars,
											STRINGA_UndoBuffer,data->buffer + data->maxchars * 2,
											GA_TabCycle,TRUE,
											TAG_MORE,ATTRLIST);
				
				if (!data->buffer || !data->stringobj)
				{
					CoerceMethod(cl,obj,OM_DISPOSE);
				} else {
					if (((struct Gadget *)data->stringobj)->Flags & GFLG_TABCYCLE)
					{
						((struct Gadget *)obj)->Flags |= GFLG_TABCYCLE;
					}
					rc = (ULONG)obj;
				}
			}
			break;

		case OM_DISPOSE:
			if (data->stringobj) DisposeObject(data->stringobj);
			if (data->buffer) FreeVec(data->buffer);

			DoSuperMethodA(cl,obj,msg);
			break;

		case PUIM_MINMAX:
			DoSuperMethodA(cl,obj,msg);

			x = adata->ri->font->tf_XSize * 4;
			y = adata->ri->fontheight;
			
			MINMAXMSG->minmax->minwidth += x;
			MINMAXMSG->minmax->defwidth += x;
			MINMAXMSG->minmax->maxwidth += PUI_MAXMAX;
			
			MINMAXMSG->minmax->minheight += y;
			MINMAXMSG->minmax->defheight += y;
			MINMAXMSG->minmax->maxheight += y;
			
			break;

		case PUIM_LAYOUT:
			DoSuperMethodA(cl,obj,msg);
			SetAttrs(data->stringobj,GA_Left,LAYOUTMSG->box.Left + adata->offsetl,
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
				DoMethodA(data->stringobj,msg);
			}
			break;
			
		case GM_GOACTIVE:
		case GM_HANDLEINPUT:
			INPUTMSG->gpi_Mouse.X -= adata->offsetl;
			INPUTMSG->gpi_Mouse.Y -= adata->offseto;

			rc = DoMethodA(data->stringobj,msg);
			
			if (rc & GMR_VERIFY)
			{
				*INPUTMSG->gpi_Termination = GAD->GadgetID;
			}
			break;

		case OM_GET:
		case OM_SET:
			rc = DoSuperMethodA(cl,obj,msg);
			rc += DoMethodA(data->stringobj,msg);
			break;
			
		case GM_GOINACTIVE:
		case OM_UPDATE:
		case OM_NOTIFY:
			rc = DoMethodA(data->stringobj,msg);
			break;


		default:
			rc = DoSuperMethodA(cl,obj,msg);
			break;

	} /* switch(msg->MethodID) */
	
	return rc;
}

