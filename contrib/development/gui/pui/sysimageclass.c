#include <OSIncludes.h>

#ifndef _AROS
#pragma header
#endif

#include <string.h>

#include "pui.h"
#include "classmacros.h"
#include "sysimageclass.h"

ULONG SysImageInstanceSize(void)
{
	return sizeof(struct sysimagedata);
}

ULONG SysImageDispatcher(struct IClass *cl,Object *obj,Msg msg)
{
	struct sysimagedata	*data;
	struct PUI_AreaData 		*adata;
	struct RastPort		*rp;
	LONG						x,y,x2,y2,m;

	ULONG						rc;
	
	rc = 0;
	
	if (msg->MethodID != OM_NEW)
	{
		data  = INST_DATA(cl,obj);
		adata = INST_DATA(areaclass,obj);
	}
	
	switch(msg->MethodID)
	{
		case OM_NEW:
			if ((obj = (Object *)DoSuperMethodA(cl,obj,msg)))
			{
				data = INST_DATA(cl,obj);
				
				data->which = GetTagData(PUIA_SysImage_Which,-1,ATTRLIST);

				if (data->which >= 0 && data->which < NUM_SYSIMAGES)
				{
					rc = (ULONG)obj;
				} else {
					CoerceMethod(cl,obj,OM_DISPOSE);
				}
			};
			break;
		
		case PUIM_MINMAX:
			DoSuperMethodA(cl,obj,msg);

			switch(data->which)
			{
				case PUIV_SysImage_CheckMark:
					x = x2 = 14;
					y = 8;y2 = PUI_MAXMAX;
					break;
			}
			
			MINMAXMSG->minmax->minwidth += x;
			MINMAXMSG->minmax->defwidth += x;
			MINMAXMSG->minmax->maxwidth += x2;
				
			MINMAXMSG->minmax->minheight += y;
			MINMAXMSG->minmax->defheight += y;
			MINMAXMSG->minmax->maxheight += y2;

			break;
		
		case GM_RENDER:
			if ((rp = RENDERMSG->gpr_RPort) && PUI_MayRender(obj))
			{
				
				switch(data->which)
				{
					case PUIV_SysImage_CheckMark:
						if (GAD->Flags & GFLG_SELECTED)
						{
							x  = GAD->LeftEdge;
							y  = GAD->TopEdge;
							x2 = x + GAD->Width - 1;
							y2 = y + GAD->Height - 1;
							m  = GAD->Height * 55 / 100;
				
							SetDrMd(rp,JAM2);
							SetAPen(rp,adata->ri->pens[SHADOWPEN]);
							DrawLine(rp,x,y+m,x+2,y2);			// = \.
							DrawLine(rp,x+1,y+m,x+3,y2);
							DrawLine(rp,x+2,y+m,x+4,y2);
							
							DrawLine(rp,x+3,y2,x2-3,y);		// =  /
							DrawLine(rp,x+4,y2,x2-2,y);		//   /
							
							DrawLine(rp,x2-1,y,x2,y);			// = ¯
																		// -----
																		//	  /¯
																		// \/  

						} /* if (!(GAD->Flags & GFLG_SELECTED)) */
						break;

				} /* switch(data->which) */

			} /* if ((rp = RENDERMSG->gpr_RPort)) */
			
			break;
			
		default:
			rc = DoSuperMethodA(cl,obj,msg);
			break;

	} /* switch(msg->MethodID) */
	
	return rc;
}



